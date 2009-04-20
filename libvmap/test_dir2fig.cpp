#include "vmap.h"

main(int argc, char* argv[]){
  if (argc<4) {
    std::cerr << "Usage: test_dir2fig <conf> <in_dir1> ... <out_fig>\n";
    exit(1);
  }

  vmap::world w;

  for (int i=2; i<argc-1; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!vmap::read_dir(argv[i], w)){
      std::cerr << "Read error.\n";
      exit(1);
    }
  }

  vmap::write_fig(argv[argc-1], vmap::legend(argv[1]), w);
}
