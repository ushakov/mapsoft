#include <fstream>
#include <string>
#include <sstream>
#include <cerrno>

#include <sys/stat.h>

#include "img_io/gobj_map.h"
#include "loaders/image_r.h"

#include "geo/geo_convs.h"
#include "geo/geo_refs.h"
#include "geo_io/geofig.h"
#include "2d/line_utils.h"

using namespace std;

namespace tiles {

iRect tile_covering(const dRect & r, int tsize){
  // диапазон плиток, накрывающих данный прямоугольник
  int tile_x1 = floor(r.x / tsize);
  int tile_y1 = floor(r.y / tsize);
  int tile_x2 = ceil((r.x + r.w) / tsize);
  int tile_y2 = ceil((r.y + r.h) / tsize);
  return iRect(tile_x1, tile_y1,
		   tile_x2 - tile_x1,
		   tile_y2 - tile_y1);
}

void PrepareOutputDir(const char* filename, string* dirname) {
  string fname(filename);

  *dirname = fname.substr(0, fname.rfind("."));

  string cmd = "mkdir -p ";
  cmd += *dirname;
  system(cmd.c_str());
}

void WriteInt(FILE* f, int n) {
  for (int i = 0; i < 4; ++i) {
    unsigned char c = n % 256;
    n >>= 8;
    fwrite(&c, 1, 1, f);
  }
}

bool write_file (const char* filename, const geo_data & world_input, const Options & opt){
  geo_data world(world_input);
  if (world.maps.empty()) return false;
  if (world.range_map().empty()) return false;

  // Note! geom is in lat/lng coordinates!
  dRect geom = opt.get<dRect>("geom");
  Proj  proj(opt.get<string>("proj", "google"));
  Datum datum(opt.get<string>("datum", "sphere"));

  double scale  = opt.get("scale",  0.0);
  double rscale = opt.get("rscale", 0.0);
  double dpi    = opt.get("dpi",    0.0);
  double factor = opt.get("factor", 1.0);
  if (rscale!=0) scale=1.0/rscale;

  // Conversion to target coordiates
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, opt);

  // setting default geometry from maps
  if (geom.empty()){
    geom=world.range_map_correct();
  }

  cerr << "geom = " << geom << endl;

  int ks_zoom = opt.get("ks",     -1);
  int gg_zoom = opt.get("google", -1);
  dPoint tiles_orig = opt.get("tiles_orig", dPoint(-180.0, 85.051128779806589));

  // tiles_orig_dest is the origin in the destination coordinates
  // we want x axis to go west-east, and y axis to go north-south
  dPoint tiles_orig_dest = tiles_orig;
  c.frw(tiles_orig_dest);

  // creating initial map reference with borders
  g_map ref; // unscaled ref
  dLine brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint pg=*p;
    dPoint pr=*p;
    c.frw(pr);
    pr.x = pr.x - tiles_orig_dest.x;
    pr.y = tiles_orig_dest.y - pr.y;
    ref.push_back(g_refpoint(pg, pr));
    ref.border.push_back(pr);
    cerr << "ref: geo " << pg << " -> raster " << pr << endl;
    cerr << "brd:  " << pr << endl;
  }
  ref.map_proj=boost::lexical_cast<string>(proj);
  ref.file=filename;
  // convert geom to dest projection
  geom = c.bb_frw(geom, geom.w/1000.0); // geom is now in projection units
  cerr << "unscaled projection geom = " << geom << endl;

  geom.x = geom.x - tiles_orig_dest.x;
  geom.y = tiles_orig_dest.y - (geom.y + geom.h);

  // calculating rescale factor for reference
  double k=0; // scale for our ref
  if ((scale!=0) && (dpi!=0)) k = scale/2.54e-2*dpi;

  // put all maps into one map_list
  g_map_list maps;
  for (vector<g_map_list>::const_iterator ml = world.maps.begin();
       ml!=world.maps.end(); ml++){
    maps.insert(maps.end(), ml->begin(), ml->end());
  }

  // fallbacks
  if (dpi==0) dpi=200;
  if (scale==0) scale=1e-5;

  if (k==0){
    if (proj.val == 4 && gg_zoom >= 0){    // Google proj and zoom level set
      double upp = - tiles_orig_dest.x / 128.0 / (1 << gg_zoom);
      k=1.0/upp;
    }
    else if (proj.val == 5 && ks_zoom > 0){
      double width = 4 * 256*(1<<(ks_zoom-2));
      double deg_per_pt = 180.0/width; // ~188
      double upp   = deg_per_pt * M_PI/180 * 6378137.0;
      k=1.0/upp;
    }
    else {
      g_map orig_ref=convs::mymap(maps);
      k=1.0/convs::map_mpp(orig_ref, ref.map_proj);
    }
  }

  cerr << "1/k=" << 1.0/k << endl;

  // rescale reference
  ref  *= k;  // ref maps to the pixel plane in dest projection
  geom *= k;  // geom is now in pixel coordinates in dest projection


  bool draw_borders = opt.get("draw_borders", false);
  GObjMAP gobj(&maps);
  if (draw_borders) gobj.show_brd();

  iRect tile;
  tile.w = 256;
  tile.h = 256;

  string dirname;
  PrepareOutputDir(filename, &dirname);

  int num_tiles = 0;
  for (int z = 0; z <= gg_zoom; ++z) {
    // prepare ref
    double scale = pow(2.0, gg_zoom - z);
    g_map zoom_ref = ref;
    dRect zoom_geom = geom;
    zoom_ref /= scale;
    zoom_geom /= scale;
    convs::map2wgs zoom_cnv(zoom_ref);

    // tiles is in tile coordinates in dest projection
    iRect tiles = tile_covering(zoom_geom, 256);
    cout << "tiles@" << z << " = " << tiles << endl;
    cout << "geom=" << zoom_geom << endl;

    gobj.set_cnv(&zoom_cnv);

    vector<int> x_coords, y_coords, zooms, offsets;
    for(int x = tiles.x; x < tiles.x + tiles.w; ++x) {
      for(int y = tiles.y; y < tiles.y + tiles.h; ++y) {
	tile.x = x * 256;
	tile.y = y * 256;
	iImage tile_image = gobj.get_image(tile);
	if (!tile_image.empty()) {
	  // make filename
	  std::stringstream ss;
	  ss << dirname
             << "/tile_"
	     << setfill('0') << setw(2) << z << "_"
	     << setw(6) << x << "_"
	     << setw(6) << y << ".jpg";
	  // save image
	  image_r::save(tile_image, ss.str().c_str(), opt);
          num_tiles++;
	}
	if (num_tiles % 100 == 0) {
	  cerr << "tiles done: " << num_tiles << " now at " << x << "," << y << endl;
	}
      }
    }
  }

  return true;
}
} //namespace
