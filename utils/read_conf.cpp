#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

#include "read_conf.h"

using namespace std;

void read_conf_usage(){
/*

This program use read_conf library. So, 

   -O <name>=<value>

   abbriviations:

   -o file	-O out_file=<file>
   -a file	-O app_file=<file>
   -c file	-O local_conf=<file>  (default "/etc/mpsf.conf")
   -C file	-O global_conf=<file> (default "~/.mpsf")

   -h		-O showhelp

You can use "--" argument to protect non-option arguments
from being treated as options:

  program <options and non-options> -- <non-options only>

*/
  exit(0);
}

bool read_conf(int argc, char **argv, Options & opts, list<string> & non_opts){
  Options cmdline_opts, global_opts, local_opts;


  /***** parse command line options to cmdline_opts *****/

  bool skip = false;

  for (int i=1; i<argc; i++){

    if (!skip){
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
        cmdline_opts["local_conf"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "-C")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["global_conf"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "--")==0){
        skip=true;
        continue;
      }

      if (argv[i][0]=='-'){
        cerr << "Uknown option: " << argv[i] << "\n";
      }

    }
    non_opts.push_back(argv[i]);
  }

  /***** reading global config file *****/

  string global_conf;

  // if global_conf was not set via command line option
  // use default value:

  if (!cmdline_opts.exists("global_conf")) 
    cmdline_opts["global_conf"]="/etc/mpsf.conf";

  cmdline_opts.get("global_conf", global_conf);

  if (global_conf != ""){
    ifstream gstr(global_conf.c_str());
    if (gstr.good()) gstr >> opts; 
    else cerr << "Can't read global_conf file: " << global_conf << "\n";
  }

  /***** reading local config file *****/

  string local_conf;

  // if local_conf was not set via command line option
  // try value from global config, then use default value:

  if (!cmdline_opts.exists("local_conf")){
    if (opts.exists("local_conf")) cmdline_opts["global_conf"] = opts["global_conf"];
    else cmdline_opts["global_conf"]="~/.mpsf";
  }

  cmdline_opts.get("local_conf",  local_conf);

  if (local_conf != ""){
    ifstream lstr(local_conf.c_str());
    if (lstr.good()) lstr >> opts;
    else cerr << "Can't read local_conf file: " << local_conf << "\n";
  }

  /***** move options from cmdline_opts to opts *****/

  for (Options::const_iterator i=cmdline_opts.begin(); i!=cmdline_opts.end(); i++){
    opts[i->first] = i->second;
  }

  return true;
}
