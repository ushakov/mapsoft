#ifndef TMAP_H
#define TMAP_H

#include <fstream>
#include <iostream>
#include "vmap/vmap.h"

/// read data from tile map dir to vmap V, return tsize

double read_tmap_data(vmap::world & V, const std::string & map_dir){

  // map tile size
  double tsize=0;
  {
    std::ifstream ff((map_dir+"/.tsize").c_str());
    ff >> tsize;
    if (tsize == 0){
      std::cerr << "can't get tsize from vmap: " << map_dir << "\n";
      exit(1);
    }
  }

  // rscale
  double rscale=0;
  {
    std::ifstream ff((map_dir+"/.rscale").c_str());
    ff >> rscale;
    if (rscale == 0){
      std::cerr << "can't get rscale from vmap: " << map_dir << "\n";
      exit(1);
    }
  }

  // style
  std::string style="";
  {
    std::ifstream ff((map_dir+"/.style").c_str());
    ff >> style;
    if (style == ""){
      std::cerr << "can't get style from vmap: " << map_dir << "\n";
      exit(1);
    }
  }
  V.rscale=rscale;
  V.style=style;
  return tsize;
}

#endif