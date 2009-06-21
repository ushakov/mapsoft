#include <fstream>
#include "vmap.h"

main(int argc, char* argv[]){
  if (argc<3) {
    std::cerr << "Usage: test_dir2mp <in_dir1> ... <out_mp>\n";
    exit(1);
  }

  vmap::world w;

  for (int i=1; i<argc-1; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!vmap::read(argv[i], w)){
      std::cerr << "Read error.\n";
      exit(1);
    }
  }

  std::ofstream of(argv[argc-1]);
  if (!of){
    std::cerr << "can't open file" << argv[argc-1] << "\n";
    return false;
  }
  mp::write(of, vmap::vmap2mp(w));
}
