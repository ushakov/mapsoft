#include <iostream>
#include "fig.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.fig\n";
    exit(0);
  }
  fig::fig_world W = fig::read(argv[1]);
  fig::write(std::cout, W);
}
