#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include "fig/fig.h"
#include "vmap.h"
#include "ocad/ocad_file.h"

namespace vmap {

using namespace std;

bool testext(const char * str, const char * ext){
    const char * pos = rindex(str, '.');
    return pos && (strcasecmp(pos+1, ext)==0);
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
  else if (testext(fname, "ocd")){
    ocad::ocad_file O;
    try{
      O.read(fname);
      return read(O);
    }
    catch (const char * msg){
      cerr << "error: bad ocad file " << fname << ": " << msg << "\n";
      return vmap::world();
    }
  }
  else if (testext(fname, "vmap")){
    ifstream IN(fname);
    return read(IN);
  }
  else{
    cerr << "error: input file extension is not fig mp ocd or vmap\n";
    return vmap::world();
  }
}

int
write(const char * fname, const world & W, const Options & O){
  if (testext(fname, "fig")){
    fig::fig_world F;
    ifstream test(fname);
    if (test.good()) fig::read(fname, F);
    write(F, W, O);
    fig::write(fname, F);
  }
  else if (testext(fname, "mp")){
    mp::mp_world M;
    ifstream test(fname);
    if (test.good()) mp::read(fname, M);
    write(M, W, O);
    mp::write(fname, M);
  }
  else if (testext(fname, "ocd")){
    ocad::ocad_file F;
    try{
      write(F, W, O);
      F.write(fname);
    }
    catch (const char * msg){
      cerr << "error: bad ocad file " << fname << "\n";
      return 0;
    }
  }
  else if (testext(fname, "vmap")){
    ofstream OUT(fname);
    write(OUT, W);
  }
  else {
    cerr << "error: output file extension is not fig mp ocd or vmap\n";
    return 0;
  }
  return 1;
}

} // namespace
