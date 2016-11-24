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
// make preview images 1:10

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_forest2fig <N> <E> <forest_dir>\n";
    exit(0);
}

main(int argc, char** argv){
  if (argc != 4) usage();

  int N = atoi(argv[1]);
  int E = atoi(argv[2]);
  std::string forest_dir = argv[3];

  ostringstream crd;
  crd << "_" << setw(2) << setfill('0') << abs(N) << (N<0?'S':'N')
      << "_" << setw(3) << setfill('0') << abs(E) << (E<0?'W':'E');

  string n_tc2000 = forest_dir + "/" + "Hansen_GFC2015_treecover2000" + crd.str() + ".tif";
  string n_loss   = forest_dir + "/" + "Hansen_GFC2015_loss" + crd.str() + ".tif";
  string n_gain   = forest_dir + "/" + "Hansen_GFC2015_gain" + crd.str() + ".tif";
  string n_year   = forest_dir + "/" + "Hansen_GFC2015_lossyear" + crd.str() + ".tif";
  string n_mask   = forest_dir + "/" + "Hansen_GFC2015_datamask" + crd.str() + ".tif";
  string n_out    = forest_dir + "/" + "Hansen_GFC2015_preview" + crd.str() + ".tif";
  string n_map    = forest_dir + "/" + "Hansen_GFC2015_preview" + crd.str() + ".map";

  iImage tc2000 = image_tiff::load(n_tc2000.c_str(), 10);
  iImage gain   = image_tiff::load(n_loss.c_str(), 10);
  iImage loss   = image_tiff::load(n_gain.c_str(), 10);
  iImage year   = image_tiff::load(n_year.c_str(), 10);
  iImage mask   = image_tiff::load(n_mask.c_str(), 10);

  iImage out(tc2000.w, tc2000.h);
  for (int y = 0; y<tc2000.h; y++){
    for (int x = 0; x<tc2000.w; x++){
      int c = tc2000.get(x,y) & 0xFF;
      int g = gain.get(x,y) & 0xFF;
      int l = loss.get(x,y) & 0xFF;
      int Y = year.get(x,y) & 0xFF;
      int m = mask.get(x,y) & 0xFF;

      if (m==2){ out.set_na(x,y, 0xFF0000); continue; }
      if (g==1){ out.set_na(x,y, 0x00FFFF); continue; }
      if (l==1){ out.set_na(x,y, 0x0000FF); continue; }
      if (c>0) { out.set_na(x,y, (256-c)<<8); continue; }
      out.set_na(x,y,0xFFFFFF);
    }
  }
  image_tiff::save(out, n_out.c_str());

  g_map ref;
  iRect rr = out.range();
  iRect r(E,N-10,10,10);
  ref.file = n_out;
  ref.map_proj = Proj("lonlat");
  ref.push_back(g_refpoint(r.TLC(), rr.BLC()));
  ref.push_back(g_refpoint(r.BLC(), rr.TLC()));
  ref.push_back(g_refpoint(r.TRC(), rr.BRC()));

  g_map_list mm;
  mm.push_back(ref);
  geo_data W;
  W.maps.push_back(mm);
  io::out(n_map, W);
}
