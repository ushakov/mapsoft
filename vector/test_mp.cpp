#include <iostream>
#include "mp.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.mp\n";
    exit(0);
  }
  mp::mp_world W = mp::read(argv[1]);
  mp::write(std::cout, W);
}
