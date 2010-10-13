#include "vmap.h"

using namespace std;

void show_help(){
  cerr << "getfig -- get part of vmap if fig format\n"
       << "Usage: getfig [options] <vmap>\n"
       << "Options:\n"
       << "  -m <rmap>  -- use specified rmap (default: \"DEFAULT\")\n"
       << "  -f <fig>   -- output filename (default: <rmap>.fig)\n"
       << "  -e         -- edit mode\n"
       << "  -p         -- print mode (default)\n"
       << "\n";
  exit(1);
}

main(int argc, char* argv[]){

  bool toedit=false;
  string rmap="DEFAULT";
  string fig="";
  int c;
  while ((c=getopt(argc, argv, "hepm:f:"))!=-1){
    switch (c){
      case 'e':  toedit=true; break;
      case 'p':  toedit=false; break;
      case 'm':  rmap=optarg; break;
      case 'f':  fig=optarg; break;
      default: show_help(); break;
    }
  }
  if (argc-optind!=1) show_help();

  vmap::vmap M;
  vmap::vmap_read(argv[optind],M);
  if (fig=="") fig=rmap+".fig";

  get_fig(fig, M, toedit, rmap);
}