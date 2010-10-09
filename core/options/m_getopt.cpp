#include <string>
#include "m_getopt.h"

using namespace std;

Options
parse_options(int argc, char **argv,
              struct option long_options[], const char * last_opt){
  Options O;
  int c;

  string optstring="+"; // note "+" in optstring
  int i = 0;
    while (long_options[i].name){
    if (long_options[i].val != 0) optstring+=long_options[i].val;
    if (long_options[i].has_arg==1)  optstring+=":";
    if (long_options[i].has_arg==2)  optstring+=";";
    i++;
  }

  while(1){
    int option_index = 0;

    c = getopt_long(argc, argv, optstring.c_str(), long_options, &option_index);
    if (c == -1) break;
    if (c == '?'){
      cerr << "error: bad option: " << argv[optind-1] << "\n";
      exit(1);
    }
    if (c == ':'){
      cerr << "error: missing argument for " << argv[optind-1] << " option\n";
      exit(1);
    }

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
  return O;
}
