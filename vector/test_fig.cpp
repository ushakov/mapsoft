#include <iostream>
#include "fig.h"

main(){
  fig::fig_world W = fig::read("test.fig");
  fig::write(std::cout, W);
}
