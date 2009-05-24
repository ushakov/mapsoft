#include "vmap.h"

main(int argc, char* argv[]){
  if (argc<4) {
    std::cerr << "Usage: test_mp2dir <in_file1> ... <out_dir>\n";
    exit(1);
  }

  mp::mp_world MP;

  for (int i=1; i<argc-1; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!mp::read(argv[i], MP)){
      std::cerr << "Read error.\n";
      exit(1);
    }
  }

  vmap::write(argv[argc-1], vmap::mp2vmap(MP));
}
