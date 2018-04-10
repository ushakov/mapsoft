#include <iostream>
#include "ocad_file.h"
#include "err/err.h"

using namespace std;
using namespace ocad;

int
main(int argc, char **argv){

  if (argc <3) {
    cerr << "usage: " << argv[0] << " <file> <file>\n";
    return 1;
  }
  ocad_file O;
  try{
    O.read(argv[1], 2);
  }
  catch (Err e){
    std::cerr << argv[1] << ": " << e.get_message() << "\n";
    return 1;
  }

  try{
    O.update_extents();
    O.write(argv[2]);
  }
  catch (Err e){
    std::cerr << argv[1] << ": " << e.get_message() << "\n";
    return 2;
  }
}
