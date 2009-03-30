#include "vmap.h"

main(int argc, char* argv[]){
  if (argc<4) {
    std::cerr << "Usage: test_fig2dir <conf> <in_file1> ... <out_dir>\n";
    exit(1);
  }

  vmap::world w;

  for (int i=2; i<argc-1; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!vmap::read_fig(argv[i], vmap::legend(argv[1]), w)){
      std::cerr << "Read error.\n";
      exit(1);
    }
  }

  vmap::write_dir(argv[argc-1], w);
}