#include "vmap.h"

main(int argc, char* argv[]){
  if (argc<2) {
    std::cerr << "Usage: mp_test <in_file1> ... <out_dir>\n";
    exit(1);
  }

  vmap::world w;

  for (int i=1; i<argc-1; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!vmap::read_mp(argv[i], vmap::legend("./mmb.cnf"), w)){
      std::cerr << "Read error.\n";
      exit(1);
    }
  }

  vmap::write_dir(argv[argc-1], w);
}