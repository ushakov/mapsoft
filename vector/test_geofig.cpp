#include <iostream>
#include "../geo_io/geofig.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.fig\n";
    exit(0);
  }
  fig::fig_world W = fig::read(argv[1]);
  g_map M=get_ref(W);

  fig::write(std::cout, W);
}
