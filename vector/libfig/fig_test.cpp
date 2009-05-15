#include "fig.h"

main(int argc, char* argv[]){

  if (argc<2) {
    std::cerr << "Usage: fig_test <in_file1> ... > <out_file>\n";
    exit(1);
  }

  fig::fig_world W;

  for (int i=1; i<argc; i++){
    std::cerr << "Reading " << argv[i] << "\n";
    if (!fig::read(argv[i], W)) exit(1);
  }

  fig::write(std::cout, W);

}