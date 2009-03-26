#include "mp.h"

main(int argc, char* argv[]){

  if (argc<2) {
    std::cerr << "Usage: mp_test <in_file1> ... > <out_file>\n";
    exit(1);
  }

  mp::mp_world W;

  for (int i=1; i<argc; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    mp::read(argv[i], W);
  }

  mp::write(std::cout, W);

}