#include "zn_key.h" // for zn::fig_old2new()

// convert old maps, with object identifires to
// new ones with label refpoints

using namespace std;


int main(int argc, char** argv){

  if (argc != 2){
    cerr << "Convert old-style fig map\n"
         << "  usage: fig_old2new <fig>\n";
    return 1;
  }
  const char * file     = argv[1];

  fig::fig_world F;
  if (!fig::read(file, F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  zn::fig_old2new(F);
  fig::write(file, F);
}
