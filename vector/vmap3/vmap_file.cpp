#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include "libfig/fig.h"
#include "vmap.h"

namespace vmap {

using namespace std;

bool testext(const char * str, const char * ext){
    const char * pos = rindex(str, '.');
    return pos && (strcmp(pos+1, ext)==0);
}

world
read(const char * fname){
  if (testext(fname, "fig")){
    fig::fig_world F;
    if (!fig::read(fname, F)){
      cerr << "error: bad fig file " << fname << "\n";
      return vmap::world();
    }
    return read(F);
  }
  else if (testext(fname, "mp")){
    mp::mp_world M;
    if (!mp::read(fname, M)){
      cerr << "error: bad mp file " << fname << "\n";
      return vmap::world();
    }
    return read(M);
  }
  else if (testext(fname, "vmap")){
    ifstream IN(fname);
    return read(IN);
  }
  else{
    cerr << "error: input file is not .fig or .mp\n";
    return vmap::world();
  }
}

int
write(const char * fname, const world & W, const Options & O){
  if (testext(fname, "fig")){
    fig::fig_world F;
    if (!fig::read(fname, F)) {
      cerr << "error: bad fig file " << fname << "\n"; 
      return 0;
    }
    write(F, W, O);
    fig::write(fname, F);
  }
  else if (testext(fname, "mp")){
    mp::mp_world M;
    mp::read(fname, M);
    write(M, W, O);
    mp::write(fname, M);
  }
  else if (testext(fname, "vmap")){
    ofstream OUT(fname);
    write(OUT, W);
  }
  else {
    cerr << "error: output file is not .fig or .mp\n";
    return 0;
  }
  return 1;
}

} // namespace
