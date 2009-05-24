#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

#include "read_conf.h"

using namespace std;

void read_conf_usage(){
/*

   -O <name>=<value>

   abbriviations:

   -o file	--out=<file>    // file for output
   -c file	--local_conf=<file>  // local config (default "/etc/mapsoft.conf")
   -C file	--global_conf=<file> // global config (default "~/.mapsoft")
   -s <tow>	--skip=<tow>         // 
      // skip maps [m], tracks [t], waypoints [w], 
      // or old (not "active log") tracks [o]

   -h		--help

You can use "--" argument to protect non-option arguments
from being treated as options:

  program <options and non-options> -- <non-options only>

*/
  exit(0);
}

bool read_conf(int argc, char **argv, Options & opts){
  Options cmdline_opts, global_opts, local_opts;


  /***** parse command line options to cmdline_opts *****/

  bool skip = false;
  StrVec args;

  for (int i=1; i<argc; i++){

    if (!skip){

      if (strcmp(argv[i], "-h")==0){
        cmdline_opts["help"] = "";
        continue;
      }

      if (strcmp(argv[i], "-o")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["out"] = argv[i];
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

      if (strcmp(argv[i], "-s")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["skip"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "-g")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["geom"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "-p")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["proj"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "-d")==0){
        if (i>=argc-1) return false;
        i+=1;
        cmdline_opts["datum"] = argv[i];
        continue;
      }

      if (strcmp(argv[i], "--")==0){
        skip=true;
        continue;
      }

      if (strncmp(argv[i], "--", 2)==0){
        istringstream istr(argv[i]+2);
        istr >> cmdline_opts;
        continue;
      }

      if (argv[i][0]=='-'){
        cerr << "Uknown option: " << argv[i] << "\n";
        continue;
      }

    }
    args.push_back(argv[i]);
  }
  if (args.size()!=0) cmdline_opts.put("cmdline_args", args);

  /***** reading global config file *****/

  string global_conf = "/etc/mapsoft.conf";
  global_conf = cmdline_opts.get("global_conf", global_conf);

  if (global_conf != ""){
    ifstream gstr(global_conf.c_str());
    if (gstr.good()) gstr >> opts;
    else cerr << "Can't read global_conf file: " << global_conf << "\n";
  }

  /***** reading local config file *****/

  string local_conf = "~/.mapsoft";
  local_conf = opts.get("local_conf", local_conf);          // try value from global_conf
  local_conf = cmdline_opts.get("local_conf", local_conf);  // owerwrite by cmdline parameters

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
