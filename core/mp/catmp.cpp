#include "mp.h"
#include "err/err.h"

int
main(int argc, char* argv[]){
  try {

    if (argc<2) {
      std::cerr << "Usage: catmp <in_file1> ... > <out_file>\n";
      return 1;
    }

    mp::mp_world W;

    for (int i=1; i<argc; i++){
      std::cerr << "Reading " << argv[i] << "\n";
      if (!mp::read(argv[i], W)) throw Err() << "can't read " << argv[i];
    }

    mp::write(std::cout, W);
  }
  catch (Err e) {
    std::cerr << e.get_error();
  }
  return 0;
}