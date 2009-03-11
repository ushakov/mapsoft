#include <fstream>
#include <string>
#include <cerrno>

#include <sys/stat.h>

#include "../layers/layer_geomap.h"
#include "../layers/layer_geodata.h"
#include "../layers/layer_google.h"
#include "../layers/layer_ks.h"

#include "../libgeo/geo_convs.h"
#include "../libgeo/geo_refs.h"
#include "../libgeo_io/geofig.h"
#include "../lib2d/line_utils.h"

#include "io_oe.h"

using namespace std;

namespace tiles {

void MakeFileNames(const char* filename, string* index_file, string* data_file, string* tmp_file) {
  string fname(filename);

  string base = fname.substr(0, fname.rfind("."));

  *index_file = base + "/index.nb";
  *data_file = base + "/map.data";
  *tmp_file = base + "/tmp.jpg";
  
  string cmd = "mkdir -p ";
  cmd += base;
  system(cmd.c_str());
}

void WriteInt(FILE* f, int n) {
  for (int i = 0; i < 4; ++i) {
    unsigned char c = n % 256;
    n >>= 8;
    fwrite(&c, 1, 1, f);
  }
}

bool write_file (const char* filename, const geo_data & world, const Options & opt){
  if (world.maps.empty()) return false;
  if (world.range_map().empty()) return false;
  
  // Note! geom is in lat/lng coordinates!
  Rect<double> geom; opt.get("geom", geom);
  Proj  proj("google");    opt.get("proj", proj);
  Datum datum("wgs84"); opt.get("datum", datum);

  double scale=0, rscale=0, dpi=0, factor=1;
  opt.get("scale",  scale);
  opt.get("rscale", rscale);
  if (rscale!=0) scale=1.0/rscale;

  // Conversion to target coordiates
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, opt);

  // setting default geometry from maps
  if (geom.empty()){
    geom=world.range_map();
  }

  cerr << "geom = " << geom << endl;

  int ks_zoom=-1; opt.get("ks",     ks_zoom);
  int gg_zoom=-1; opt.get("google", gg_zoom);
  g_point tiles_orig(-180.0, 85.051128779806589);  opt.get("tiles_orig", tiles_orig);

  geom = c.bb_frw(geom, geom.w/1000.0); // geom is now in projection units
  cerr << "unscaled projection geom = " << geom << endl;

  // tiles_orig_dest is the origin in the destination coordinates
  // we want x axis to go west-east, and y axis to go north-south
  g_point tiles_orig_dest = tiles_orig;
  c.frw(tiles_orig_dest);

  // creating initial map reference with borders
  g_map ref; // unscaled ref
  g_line brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (g_line::const_iterator p=brd.begin(); p!=brd.end(); p++){
    g_point pg=*p;
    g_point pr=*p;
    c.frw(pr);
    pr.x = pr.x - tiles_orig_dest.x;
    pr.y = tiles_orig_dest.y - pr.y;
    ref.push_back(g_refpoint(pg, pr));
    ref.border.push_back(pr);
  }
  ref.map_proj=boost::lexical_cast<string>(proj);
  ref.file=filename;
  // convert geom to dest projection
  geom.x = geom.x - tiles_orig_dest.x;
  geom.y = tiles_orig_dest.y - (geom.y + geom.h);

  // calculating rescale factor for reference
  double k=0; // scale for our ref
  if ((scale!=0) && (dpi!=0)) k = scale/2.54e-2*dpi;

  // fallbacks
  if (dpi==0) dpi=200;
  if (scale==0) scale=1e-5;

  if (k==0){
    if (proj.val == 4 && gg_zoom >= 0){    // Google proj and zoom level set
      double upp = 360.0 / 256.0 / (1 << gg_zoom);
      k=1.0/upp;
    }
    else if (proj.val == 5 && ks_zoom > 0){
      double width = 4 * 256*(1<<(ks_zoom-2));
      double deg_per_pt = 180.0/width; // ~188
      double upp   = deg_per_pt * M_PI/180 * 6378137.0;
      k=1.0/upp;
    }
    else {
      g_map orig_ref=convs::mymap(world);
      k=1.0/convs::map_mpp(orig_ref, ref.map_proj);
    }
  }

  cerr << "1/k=" << 1.0/k << endl;

  // rescale reference
  ref  *= k;  // ref maps to the pixel plane in dest projection
  geom *= k;  // geom is now in pixel coordinates in dest projection

  bool draw_borders=false;  opt.get("draw_borders", draw_borders);
  LayerGeoMap layer(&world, draw_borders);

  Rect<int> tile;
  tile.w = 256;
  tile.h = 256;

  string index_file, data_file, tmp_file;
  MakeFileNames(filename, &index_file, &data_file, &tmp_file);

  FILE * index = fopen(index_file.c_str(), "wb");
  if (!index) {
    cerr << "cannot open file " << index_file << " for writing!" << endl;
    exit(1);
  }
  // write placeholder for total elements
  WriteInt(index, 0);
  size_t offset = 0;
  size_t num_tiles = 0;

  string cat_cmd = "cat ";
  cat_cmd += tmp_file + " >> " + data_file;
  string rm_cmd = "rm " + data_file;
  system(rm_cmd.c_str());
  
  for (int z = 0; z <= gg_zoom; ++z) {
    // prepare ref
    double scale = pow(2.0, gg_zoom - z);
    g_map zoom_ref = ref;
    Rect<double> zoom_geom = geom;
    zoom_ref /= scale;
    zoom_geom /= scale;
    
    // tiles is in tile coordinates in dest projection
    Rect<int> tiles = tiles_on_rect(zoom_geom, 256);
    cout << "tiles@" << z << " = " << tiles << endl;
    cout << "geom=" << zoom_geom << endl;

    layer.set_ref(zoom_ref);

    for(int x = tiles.x; x < tiles.x + tiles.w; ++x) {
      for(int y = tiles.y; y < tiles.y + tiles.h; ++y) {
	tile.x = x * 256;
	tile.y = y * 256;
	Image<int> tile_image = layer.get_image(tile);
	if (!tile_image.empty()) {
	  // save image
	  image_r::save(tile_image, tmp_file.c_str(), opt);
	  // get file size
	  struct stat st;
	  stat(tmp_file.c_str(), &st);
	  size_t len = st.st_size;

	  // write index
	  WriteInt(index, z);
	  WriteInt(index, y);
	  WriteInt(index, x);
	  WriteInt(index, offset);
	  WriteInt(index, len);

	  // append to data file
	  int r = system(cat_cmd.c_str());
	  if (r != 0) {
	    cerr << "cat returned " << r << " errno=" << errno << endl;
	    exit(1);
	  }
	  offset += len;
	  num_tiles ++;

	  stat(data_file.c_str(), &st);
	  cerr << "file size - offset = " << (int)st.st_size - offset << endl;
	}
	if (num_tiles % 100 == 0) {
	  cerr << "tiles done: " << num_tiles << " now at " << x << "," << y << endl;
	}
      }
    }
  }
  
  fseek(index, 0, SEEK_SET);
  WriteInt(index, num_tiles);
  cerr << "total " << num_tiles << " written" << endl;
  fclose(index);

  return true;
}
} //namespace
