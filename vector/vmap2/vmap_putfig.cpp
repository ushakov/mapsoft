#include "vmap.h"
using namespace std;

void show_help(){
  cerr << "putfig -- insert fig-file into vmap\n"
       << "Usage: putfig [options] <vmap> <fig1> ...\n"
       << "Options:\n"
       << "  -a         -- add mode\n"
       << "  -r         -- replace mode (default)\n"
       << "Note: for replace mode fig must be prepared with getfig -e"
       << "\n";
  exit(1);
}

main(int argc, char* argv[]){

  bool replace=true;
  string rmap="DEFAULT";
  int c;
  while ((c=getopt(argc, argv, "har"))!=-1){
    switch (c){
      case 'a':  replace=false; break;
      case 'r':  replace=true; break;
      default: show_help(); break;
    }
  }
  if (argc-optind<1) show_help();

  vmap::vmap M;
  vmap::vmap_read(argv[optind],M);

  for (int i=optind+1; i<argc; i++){
    cerr << "reading " << argv[i] << "\n";
    vmap::put_fig(argv[i], M, replace);
  }
  vmap::vmap_write(argv[optind],M);
}