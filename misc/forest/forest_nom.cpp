#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <cmath>

#include "srtm/srtm3.h"
#include "2d/line.h"
#include "geo/geo_nom.h"

#include "2d/line_utils.h"
#include "2d/line_polycrop.h"
#include "2d/point_int.h"

#include "geo_io/geofig.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"

#include "loaders/image_tiff.h"
#include "loaders/image_jpeg.h"
#include "geo_io/io.h"

// Global Forest Change 2000-2014
// http://earthenginepartners.appspot.com/science-2013-global-forest/download_v1.2.html

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_forest2fig <name> <forest_dir>\n";
    exit(0);
}

main(int argc, char** argv){
  if (argc < 3) usage();

  std::string name = argv[1];
  std::string forest_dir = argv[2];

  dRect range = convs::nom_to_range(name);

// file names:
// Hansen_GFC2015_datamask_40S_080W.tif
// Hansen_GFC2015_gain_40S_080W.tif
// Hansen_GFC2015_last_40S_080W.tif
// Hansen_GFC2015_loss_40S_080W.tif
// Hansen_GFC2015_lossyear_40S_080W.tif
// Hansen_GFC2015_treecover2000_40S_080W.tif

  // load data (10x10deg, 1"x1" points)

  int deg2pt = 4000;
  int tsize  = 40000;
  iRect irange = range*deg2pt;
  iRect tiles = tiles_on_rect(irange, tsize);

  iImage tc2000(irange.w, irange.h);
  iImage gain(irange.w, irange.h);
  iImage loss(irange.w, irange.h);
  iImage year(irange.w, irange.h);
  iImage mask(irange.w, irange.h);

  for (int yt = tiles.y; yt<tiles.y+tiles.h; yt++){
    for (int xt = tiles.x; xt<tiles.x+tiles.w; xt++){
      ostringstream crd;
      crd << "_" << setw(2) << setfill('0') << abs((yt+1)*10) << (yt+1<0?'S':'N')
          << "_" << setw(3) << setfill('0') << abs(xt*10) << (xt<0?'W':'E');
      iRect load = irange-iPoint(xt,yt)*tsize;
      load.y = tsize-load.y-load.h;
      string n_tc2000 = forest_dir + "/" + "Hansen_GFC2015_treecover2000" + crd.str() + ".tif";
      string n_loss   = forest_dir + "/" + "Hansen_GFC2015_loss" + crd.str() + ".tif";
      string n_gain   = forest_dir + "/" + "Hansen_GFC2015_gain" + crd.str() + ".tif";
      string n_year   = forest_dir + "/" + "Hansen_GFC2015_lossyear" + crd.str() + ".tif";
      string n_mask   = forest_dir + "/" + "Hansen_GFC2015_datamask" + crd.str() + ".tif";

      image_tiff::load(n_tc2000.c_str(), load, tc2000, tc2000.range());
      image_tiff::load(n_loss.c_str(), load, loss, loss.range());
      image_tiff::load(n_gain.c_str(), load, gain, gain.range());
      image_tiff::load(n_year.c_str(), load, year, year.range());
      image_tiff::load(n_mask.c_str(), load, mask, mask.range());
    }
  }

  g_map ref;
  ref.file = name + ".tif";
  ref.map_proj = Proj("lonlat");
  ref.clear();
  ref.border.clear();
  ref.push_back(g_refpoint(range.TLC(), irange.BLC()-irange.TLC()));
  ref.push_back(g_refpoint(range.BLC(), irange.TLC()-irange.TLC()));
  ref.push_back(g_refpoint(range.TRC(), irange.BRC()-irange.TLC()));

  g_map_list mm;
  mm.push_back(ref);
  geo_data W;
  W.maps.push_back(mm);
  io::out(name + ".map", W);

  for (int y = 0; y<tc2000.h; y++){
    for (int x = 0; x<tc2000.w; x++){
      int c = tc2000.get(x,y) & 0xFF;
      int g = gain.get(x,y) & 0xFF;
      int l = loss.get(x,y) & 0xFF;
      int Y = year.get(x,y) & 0xFF;
      int m = mask.get(x,y) & 0xFF;

//      int v = 0xAA + (0xFF-0xAA)*(100-c)/100;
//      if (m==2){ tc2000.set_na(x,y, 0xFFFF00); continue; }
//      if (g==1){ tc2000.set_na(x,y, 0xAAEEDD); continue; }
//      if (l==1){ tc2000.set_na(x,y, 0x88BB88); continue; }
//      if (c>0) { tc2000.set_na(x,y, (v<<16)+0xff00+v); continue; }
//      tc2000.set_na(x,y,0xFFFFFF);

      if (m==2){ tc2000.set_na(x,y, 0xFFFF00); continue; }
      if (g==1 || l==1){ tc2000.set_na(x,y, 0xAAEEDD); continue; }
//      if (l==1){ tc2000.set_na(x,y, 0x88BB88); continue; }
      if (c>0 && c<50)  { tc2000.set_na(x,y, 0xddffdd); continue; }
      if (c>50) { tc2000.set_na(x,y, 0xaaffaa); continue; }
      tc2000.set_na(x,y,0xFFFFFF);

    }
  }
  image_tiff::save(tc2000, (name + ".tif").c_str());
}
