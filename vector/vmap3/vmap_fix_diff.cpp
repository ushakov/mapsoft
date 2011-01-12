#include <string>
#include <fstream>
#include <iostream>
#include "vmap/vmap.h"

using namespace std;

void usage(){
  const char * prog = "vmap_fix_diff";

  cerr
     << prog << " -- remove inessential difference between old and new maps.\n"
     << "  usage: " << prog << " <old map> <new map> <out map>\n"
     << "\n"
  ;
  exit(1);
}

main(int argc, char **argv){

  if (argc!=4) usage();
  const char *in1 = argv[1];
  const char *in2 = argv[2];
  const char *out = argv[3];

  vmap::world V1 = vmap::read(in1);
  vmap::world V2 = vmap::read(in2);
  vmap::fix_diff(V1,V2, 1e-4);
  vmap::remove_dups(V2, 1e-5);

  vmap::write(out, V2);
  exit(0);
}


