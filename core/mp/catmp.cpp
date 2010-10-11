#include "mp.h"

main(int argc, char* argv[]){

  if (argc<2) {
    std::cerr << "Usage: catmp <in_file1> ... > <out_file>\n";
    exit(1);
  }

  mp::mp_world W;

  for (int i=1; i<argc; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!mp::read(argv[i], W)) exit(1);
  }

  mp::write(std::cout, W);

}