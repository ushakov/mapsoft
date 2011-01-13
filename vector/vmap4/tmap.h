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

dRect read_rmap_geom(const std::string & map_dir, const std::string & name){
  dRect geom;
  if (name == "") return geom;
  std::ifstream ff((map_dir+"/"+name+"/geom").c_str());
  if (ff.good()) ff >> geom;
  return geom;
}
std::string read_rmap_title(const std::string & map_dir, const std::string & name){
  std::string title;
  if (name == "") return title;
  std::ifstream ff((map_dir+"/"+name+"/title").c_str());
  if (ff.good()) ff >> title;
  return title;
}
vmap::world read_rmap_labels(const std::string & map_dir, const std::string & name){
  std::string fname=map_dir+"/"+name+"/labels.vmap";
  std::ifstream ff(fname.c_str());
  if (ff.good()) return vmap::read(fname.c_str());
  return vmap::world();
}

#endif
