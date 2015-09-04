#include <iostream>

#include "io.h"
#include "geo_io/io.h"
#include "o_img.h"
#include "o_tiles.h"

namespace io {
using namespace std;

void
out_img(const string & out_name, geo_data & world, vmap::world & vm, const Options & opt){

  string name(out_name);

  // write raster image
  if ((testext(name, ".tiff")) ||
      (testext(name, ".tif")) ||
      (testext(name, ".png")) ||
      (testext(name, ".jpeg")) ||
      (testext(name, ".jpg")) ){

    cerr << "Writing image " << name << "\n";
    img::write_file(name.c_str(), world, vm, opt);
    return;
  }

  // write tiles
  if (testext(name, ".tiles")) {
    cerr << "Writing tiles to " << name << "\n";
    tiles::write_file(name.c_str(), world, opt);
    return;
  }

  cerr << "Can't write image with name " << name << "\n"
       << "Use (.jpg, .jpeg, .png, .tif, .tiff or .tiles) extensions\n";
}

}// namespace


