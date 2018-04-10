#include "fig.h"
#include "err/err.h"

main(int argc, char* argv[]){
  try {

    if (argc<2) {
      std::cerr << "Usage: catfig <in_file1> ... > <out_file>\n";
      return 1;
    }

    fig::fig_world W;

    for (int i=1; i<argc; i++){
      std::cerr << "Reading " << argv[i] << "\n";
      if (!fig::read(argv[i], W)) throw Err() << "can't read " << argv[i];
    }

    fig::write(std::cout, W);
  }
  catch (Err e) {
    std::cerr << e.get_error();
  }

}