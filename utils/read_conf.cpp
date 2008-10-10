#include <fstream>
#include <sstream>
#include <cstring>

#include "read_conf.h"

using namespace std;

void read_conf_usage(){
/*

   -O <name>=<value>

   abbriviations:

   -o file	-O out_file=<file>
   -a file	-O app_file=<file>
   -c file	-O local_conf=<file>  (default "/etc/mpsf.conf")
   -C file	-O global_conf=<file> (default "~/.mpsf")

   -h		-O showhelp

*/
  exit(0);
}

bool read_conf(int argc, char **argv, Options & opts, std::vector<std::string> & non_opts){
  Options cmdline_opts, global_opts, local_opts;

  cmdline_opts["global_conf"]="/etc/mpsf.conf";
  cmdline_opts["local_conf"]="~/.mpsf";

// разбор командной строки
  for (int i=1; i<argc; i++){

    if (strcmp(argv[i], "-O")==0){
      if (i>=argc-1) return false;
      i+=1;
      istringstream istr(argv[i]);
      istr >> cmdline_opts;
      continue;
    }

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)){
      cmdline_opts["showhelp"] = argv[i];
      continue;
    }

    if (strcmp(argv[i], "-o")==0){
      if (i>=argc-1) return false;
      i+=1;
      cmdline_opts["out_file"] = argv[i];
      continue;
    }

    if (strcmp(argv[i], "-a")==0){
      if (i>=argc-1) return false;
      i+=1;
      cmdline_opts["app_file"] = argv[i];
      continue;
    }

    if (strcmp(argv[i], "-c")==0){
      if (i>=argc-1) return false;
      i+=1;
      cmdline_opts["local_config"] = argv[i];
      continue;
    }

    if (strcmp(argv[i], "-C")==0){
      if (i>=argc-1) return false;
      i+=1;
      cmdline_opts["global_config"] = argv[i];
      continue;
    }

    non_opts.push_back(argv[i]);
  }

  std::string global_config, local_config;

  cmdline_opts.get("global_config", global_config);
  cmdline_opts.get("local_config",  local_config);
  ifstream gstr(global_config.c_str());
  ifstream lstr(local_config.c_str());

  if (gstr) gstr >> opts;
  if (lstr) lstr >> opts;
  for (Options::const_iterator i=cmdline_opts.begin(); i!=cmdline_opts.end(); i++){
    opts[i->first] = i->second;
  }

  return true;
}
