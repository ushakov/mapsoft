#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <cstring>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "geo_io/io.h"
#include "img_io/io.h"
#include "options/m_getopt.h"
#include "err/err.h"

using namespace std;

#define OPT1  1  //
#define OPT2  2  // geometry options 
#define OPT3  4  // other raster options
#define OPT4  8  //
#define OPT_ALL  (OPT1 | OPT2 | OPT3 | OPT4)

static struct ext_option options[] = {
  {"out",                   1,'o', OPT1, "output file name"},
  {"help",                  0,'h', OPT1, "show this message"},
  {"pod",                   0,  0, OPT1, "show this message as POD template"},
  {"verbose",               0,'v', OPT1, "be verbose\n"},

  {"shift_maps",            1,  0, OPT1, "shift map references, \"x,y\""},
  {"rescale_maps",          1,  0, OPT1, "rescale map references"},
  {"map_nom_brd",           0,  0, OPT1, "set map borders according to map name"},
  {"map_ref_brd",           0,  0, OPT1, "set map borders according to map refpoints"},
  {"skip",                  1,'s', OPT1, "skip data, \"wmtao\" (w - waypoints, m - maps, t - tracks, a - active log, o - save tracks)"},
  {"gen_n",                 1,  0, OPT1, "reduce track points to n"},
  {"gen_e",                 1,  0, OPT1, "reduce track points up to accuracy e [meters] (when gen_n and gen_e both used it means: \"remove points while number of points > n OR accuracy < e\""},
  {"join_maps",             0,  0, OPT1, "join all maps into one group"},
  {"join_wpts",             0,  0, OPT1, "join all waypoints into one group"},
  {"join_trks",             0,  0, OPT1, "join all tracks into one"},

  {"geom",          1,  0, OPT2, ""},
  {"datum",         1,  0, OPT2, ""},
  {"proj",          1,  0, OPT2, ""},
  {"lon0",          1,  0, OPT2, ""},
  {"wgs_geom",      1,  0, OPT2, ""},
  {"wgs_brd",       1,  0, OPT2, ""},
  {"trk_brd",       1,  0, OPT2, ""},
  {"nom",           1,  0, OPT2, ""},
  {"google",        1,  0, OPT2, "google tile, \"x,y,z\""},
  {"rscale",        1,  0, OPT2, "reversed scale (10000 for 1:10000 map)"},
  {"dpi",           1,'d', OPT2, "resolution, dots per inch"},
  {"mag",           1,  0, OPT2, "additional magnification"},
  {"swap_y",        0,  0, OPT2, ""},

  {"htm",           1,  0, OPT3, "write html map into file"},
  {"fig",           1,'f', OPT3, "write fig-file"},
  {"map",           1,'m', OPT3, "write map-file"},
  {"map_show_brd",  0,  0, OPT3, "draw map borders"},

  {"trk_draw_mode" ,1,  0, OPT3, "draw mode: normal, speed, height"},
  {"trk_draw_dots" ,0,  0, OPT3, "draw dots on tracks"},
  {"trk_draw_arrows",0, 0, OPT3, "draw arrows on tracks"},
  {"trk_draw_v1"   ,1,  0, OPT3, "low value for speed draw mode"},
  {"trk_draw_v2"   ,1,  0, OPT3, "high value for speed draw mode"},
  {"trk_draw_h1"   ,1,  0, OPT3, "low value for height draw mode"},
  {"trk_draw_h2"   ,1,  0, OPT3, "high value for height draw mode"},

  {"max_image",     1,  0, OPT3, "don't write images larger then this, \"x,y\", default 1000,1000"},
  {"data_marg",     1,  0, OPT3, "margins around data (works only if no geometry set), pixels"},
  {"jpeg_quality",  1,  0, OPT3, "set jpeg quality"},
  {"bgcolor",       1,  0, OPT3, "backgound color"},
  {"tiles",         0,  0, OPT3, "write tiles"},
  {"tiles_origin",  1,  0, OPT3, "origin of tiles \"image\" (default) or \"proj\""},
  {"tiles_skipempty",0, 0, OPT3, "skip empty tiles"},

  {"srtm_mode" ,    1,  0, OPT4, "srtm mode: normal, slopes"},
  {"srtm_dir" ,     1,  0, OPT4, "srtm data folder"},
  {"srtm_cnt_step", 1,  0, OPT4, "contour step, m"},
  {"srtm_hmin",     1,  0, OPT4, "min height (blue color) for normal mode"},
  {"srtm_hmax",     1,  0, OPT4, "max height (magenta color) for normal mode"},
  {"srtm_smin",     1,  0, OPT4, "min slope for slopes mode (30)"},
  {"srtm_smax",     1,  0, OPT4, "max slope for slopes mode (55)"},

  {0,0,0,0}
};

void usage(bool pod=false){

  string head = pod? "\n=head1 ":"\n";

  const char * fname = "mapsoft_convert";
  cerr << fname << " -- convert geodata between different formats\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options> <input files> -o <output file>\n"
       << head << "Input files (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.fig -- mapsoft geofig format\n"
       << "  *.wpt, *.plt, *.map -- OziExplorer format\n"
       << "  *.gpx -- GPX format /partial support, tracks and waypoints/\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  *.zip -- zipped files\n"
       << " gps: --  read data from Garmin GPS via autodetected serial device\n"
       << " <character device> -- read data from Garmin GPS via serial device\n"
       << head << "Output file (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.wpt, *.plt, *.map, *.oe  -- OziExplorer format\n"
       << "      (Each track, waypoint set or map reference will be written in a\n"
       << "       separate .plt, .wpt or .map file)\n"
       << "  *.zip -- zipped OziExplorer files\n"
       << "  *.gpx -- GPX format /partial support, tracks and waypoints/\n"
       << "  *.kml -- Google KML format /partial support, tracks and waypoints/\n"
       << "  *.kmz -- zipped kml\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  *.tif, .tiff, .jpg, .jpeg -- raster image (map, fig, or html wrap can be made)\n"
       << " gps: --  send data to Garmin GPS via autodetected serial device\n"
       << " <character device> -- send data to Garmin GPS via serial device\n"
  ;
  cerr << head << "Options:\n";
  print_options(options, OPT1, cerr, pod);
  cerr << head << "Options for rendering images:\n";
  print_options(options, OPT3, cerr, pod);
  cerr << head << "Options for rendering images, geometry settings:\n";
  print_options(options, OPT2, cerr, pod);
  cerr << head << "Options for rendering images, SRTM input:\n";
  print_options(options, OPT4, cerr, pod);

  exit(1);
}

int main(int argc, char *argv[]) {
try{

  if (argc==1) usage();

  vector<string> infiles;
  Options O = parse_options_all(&argc, &argv, options, OPT_ALL, infiles);
  if (O.exists("help")) usage();
  if (O.exists("pod")) usage(true);

  if (!O.exists("out")){
    cerr << "no output files.\n";
    exit(1);
  }

  geo_data world;
  vmap::world vm;
  for (vector<string>::const_iterator i = infiles.begin(); i!=infiles.end(); i++){
    if (io::testext(*i, ".vmap")) vm = vmap::read(i->c_str());
    else io::in(*i, world, O);
  }

  if (O.exists("verbose")){
    cerr << "Map lists: " << world.maps.size()
         << ",  Waypoint lists: " << world.wpts.size()
         << ",  Tracks: " << world.trks.size() << "\n";
  }

  if (O.exists("verbose")) cerr << "Applying filters...\n";

  io::filter(world, O);

  string name=O.get("out", string());
  if ((io::testext(name, ".tiff")) ||
      (io::testext(name, ".tif")) ||
      (io::testext(name, ".png")) ||
      (io::testext(name, ".jpeg")) ||
      (io::testext(name, ".jpg")) ||
      (io::testext(name, ".tiles")) 
     ){
    io::out_img(name, world, vm, O);
  }
  else {
    io::out(name, world, O);
  }
}
catch (Err e) {
  cerr << e.get_error() << endl;
  return 1;
}

}

