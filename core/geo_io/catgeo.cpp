#include "io.h"
#include "err/err.h"

main(int argc, char* argv[]){

  if (argc<3) {
    std::cerr << "Usage: catgeo <in_file1> ... <out_file>\n";
    exit(1);
  }

  geo_data W;

  try {
    for (int i=1; i<argc-1; i++){
      std::cerr << "Reading " << argv[i] << "\n";
      io::in(argv[i], W);
    }
    io::out(argv[argc-1], W);
  } catch (Err e){
    std::cerr << e.get_error() << std::endl;
    exit(1);
  }
  exit(0);
}