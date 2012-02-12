#include <string>
#include <sstream>
#include <iomanip>
#include "m_getopt.h"

using namespace std;

Options
parse_options(int * argc, char ***argv,
              struct option long_options[], const char * last_opt){
  Options O;
  int c;

  string optstring="+"; // note "+" in optstring
  int i = 0;
  while (long_options[i].name){
    if (long_options[i].val != 0){ optstring+=long_options[i].val;
      if (long_options[i].has_arg==1)  optstring+=":";
      if (long_options[i].has_arg==2)  optstring+="::";
    }
    if (long_options[i].flag){
      cerr << "error: non-zero flag in option structure\n";
      exit(1);
    }
    i++;
  }

  while(1){
    int option_index = 0;

    c = getopt_long(*argc, *argv, optstring.c_str(), long_options, &option_index);
    if (c == -1) break;
    if ((c == '?') || (c == ':'))  exit(1);

    if (last_opt && O.exists(last_opt)){
      cerr << "error: wrong position of --" << last_opt << " special option\n";
      exit(1);
    }

    if (c!=0){ // short option -- we must manually set option_index
      int i = 0;
      while (long_options[i].name){
        if (long_options[i].val == c) option_index = i;
        i++;
      }
    }
    if (!long_options[option_index].name){
      cerr << "error: bad option\n";
      exit(1);
    }
    O.put<string>(long_options[option_index].name,
      long_options[option_index].has_arg? optarg:"1");

  }
  *argc-=optind;
  *argv+=optind;
  optind=0;

  return O;
}


/**********************************************/
Options
parse_options(int *argc, char ***argv,
              struct ext_option ext_options[],
              int mask,
              const char * last_opt){
  // get number of options
  int num;
  for (num=0; ext_options[num].name; num++){ }

  // build long_options structure
  option * long_options = new option[num+1];
  int i,j;
  for (i=0, j=0; i<num; i++){
    if ((ext_options[i].group & mask) == 0) continue;
    long_options[j].name    = ext_options[i].name;
    long_options[j].has_arg = ext_options[i].has_arg;
    long_options[j].flag    = NULL;
    long_options[j].val     = ext_options[i].val;
    j++;
  }
  long_options[j].name    = NULL;
  long_options[j].has_arg = 0;
  long_options[j].flag    = NULL;
  long_options[j].val     = 0;

  Options O = parse_options(argc, argv, long_options, last_opt);

  delete[] long_options;
  return O;
}

void
print_options(struct ext_option ext_options[],
              int mask, std::ostream & s, bool pod){
  const int option_width = 25;
  const int indent_width = option_width+4;
  const int text_width = 77-indent_width;

  for (int i = 0; ext_options[i].name; i++){
    if ((ext_options[i].group & mask) == 0) continue;

    ostringstream oname;

    if (ext_options[i].val)
      oname << " -" << (const char)ext_options[i].val << ",";
    oname << " --" << ext_options[i].name;

    if (ext_options[i].has_arg == 1) oname << " <arg>";
    if (ext_options[i].has_arg == 2) oname << " [<arg>]";

    string desc(ext_options[i].desc);

    if (!pod){
      s << setw(option_width) << oname.str() << " -- ";

      int lsp=0;
      int ii=0;
      for (int i=0; i<desc.size(); i++,ii++){
        if ((desc[i]==' ') || (desc[i]=='\n')) lsp=i+1;
        if ((ii>=text_width) || (desc[i]=='\n')){
          if (lsp <= i-ii) lsp = i;
          if (ii!=i) s << string(indent_width, ' ');
          s << desc.substr(i-ii, lsp-i+ii-1) << endl;
          ii=i-lsp;
        }
      }
      if (ii!=desc.size()) s << string(indent_width, ' ');
      s << desc.substr(desc.size()-ii, ii) << "\n";
    }
    else {
      s << "\nB<< " << oname.str() << " >> -- " << desc << "\n";
    }

  }
}
