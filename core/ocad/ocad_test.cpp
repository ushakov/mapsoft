#include <iostream>
#include "ocad_file.h"

using namespace std;
using namespace ocad;

int
main(int argc, char **argv){
  if (argc <3) {
    cerr << "usage: " << argv[0] << " <file> <file>\n";
    exit(1);
  }
  ocad_file O;
  try{
    O.read(argv[1], 2);
  }
  catch (const char * msg){
    std::cerr << argv[1] << ": " << msg << "\n";
    exit(1);
  }

  try{
    O.update_extents();
    O.write(argv[2]);
  }
  catch (const char * msg){
    std::cerr << argv[2] << ": " << msg << "\n";
    exit(1);
  }
}
