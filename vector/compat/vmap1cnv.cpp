#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <cstdlib>

#include "vmap/vmap.h"

// convert broken angles (old fig->vmap->fig conversion)
main(int argc, char **argv){
  if (argc<2){
    std::cout << "no input files\n";
    exit(1);
  }
  vmap::world V = vmap::read(argv[1]);

  try {
    for (vmap::world::iterator o=V.begin(); o!=V.end(); o++){
      for (std::list<vmap::lpos>::iterator l=o->labels.begin(); l!=o->labels.end(); l++){
        if (l->hor) continue;
//        l->ang -= 3.71*(l->pos.x - convs::get_lon0(l->pos))*sin(l->pos.y * M_PI/180.0);
        l->ang += 2.3*(l->pos.x - convs::get_lon0(l->pos));
      }
    }
  } catch (const char *x){
    std::cerr << "Error: " << x << "\n";
    exit(1);
  }


  vmap::write(argv[1], V);
}
