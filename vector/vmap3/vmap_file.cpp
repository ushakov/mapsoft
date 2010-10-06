#include <string>
#include <iostream>
#include <cstring>

#include "libfig/fig.h"
#include "vmap.h"

namespace vmap {

using namespace std;

bool testext(const char * str, const char * ext){
    const char * pos = rindex(str, '.');
    return pos && (strcmp(pos+1, ext)==0);
}

int world::get(const char * fname, const Options & O){
  if (testext(fname, "fig")){
    fig::fig_world F;
    if (!fig::read(fname, F)){
      cerr << "error: bad fig file " << fname << "\n";
      return 0;
    }
    if (!get(F, O)){
      cerr << "error: can't read vmap from fig file " << fname << "\n";
      return 0;
    }
  }
  else if (testext(fname, "mp")){
    mp::mp_world M;
    if (!mp::read(fname, M)){
      cerr << "error: bad mp file " << fname << "\n";
      return 0;
    }
    if (!get(M, O)){
      cerr << "error: can't read vmap from mp file " << fname << "\n";
      return 0;
    }
  }
  else{
    cerr << "error: input file is not .fig or .mp\n";
    return 0;
  }
  return 1;
}
int world::put(const char * fname, const Options & O){
  if (testext(fname, "fig")){
    fig::fig_world F;
    if (!fig::read(fname, F)) {
      cerr << "error: bad fig file " << fname << "\n"; 
      return 0;
    }
    put(F, O);
    fig::write(fname, F);
  }
  else if (testext(fname, "mp")){
    mp::mp_world M;
    mp::read(fname, M);
    put(M, O);
    mp::write(fname, M);
  }
  else {
    cerr << "error: output file is not .fig or .mp\n";
    return 0;
  }
  return 1;
}

} // namespace
