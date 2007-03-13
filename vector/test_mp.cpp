#include <iostream>
#include "mp.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.mp\n";
    exit(0);
  }


  mp::mp_world W = mp::read(argv[1]);
  mp::mp_world W1;

  for (mp::mp_world::const_iterator i=W.begin(); i!=W.end(); i++){
    if (test_object(*i, "POI 0x5905 0 1"))
      W1.push_back(*i);
  }

  mp::write(std::cout, W1);
}
