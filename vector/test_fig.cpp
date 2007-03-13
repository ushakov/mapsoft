#include <iostream>
#include "fig.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.fig\n";
    exit(0);
  }
  fig::fig_world W = fig::read(argv[1]);
  std::cerr << W.size() << " objects\n";

  fig::fig_world W1;

  // выбрать все объекты такого вида:
  for (fig::fig_world::const_iterator i=W.begin(); i!=W.end(); i++){
    if (test_object(*i, "3 * * * 26 * 90 * * * * * * *"))
      W1.push_back(*i);
  }

  fig::write(std::cout, W1);
}
