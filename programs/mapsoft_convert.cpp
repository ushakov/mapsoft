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
#include "options/m_getopt.h"

using namespace std;

#define OPT1  1  //
#define OPT2  2  // geometry options 
#define OPT3  4  // other raster options
#define OPT4  8  // google/ks options
#define OPT_ALL  (OPT1 | OPT2 | OPT3 | OPT4)

static struct ext_option options[] = {
  {"out",                   1,'o', OPT1, ""},
  {"help",                  0,'h', OPT1, "show this message"},
  {"verbose",               0,'v', OPT1, "be verbose\n"},

  {"shift_maps",            1,  0, OPT1, "shift map references, \"x,y\""},
  {"rescale_maps",          1,  0, OPT1, "rescale map references"},
  {"map_nom_brd",           0,  0, OPT1, "set map borders according to map name"},
  {"skip",                  1,'s', OPT1, "skip data, \"wmtao\" (w - waypoints, m - maps, t - tracks, a - active log, o - save tracks)"},
  {"gen_n",                 1,  0, OPT1, "reduce track points to n"},
  {"gen_e",                 1,  0, OPT1, "reduce track points up to accuracy e [meters] (when gen_n and gen_e both used it means: \"remove points while number of points > n OR accuracy < e\""},

  {"geom",          1,  0, OPT2, ""},
  {"datum",         1,  0, OPT2, ""},
  {"proj",          1,  0, OPT2, ""},
  {"lon0",          1,  0, OPT2, ""},
  {"wgs_geom",      1,  0, OPT2, ""},
  {"wgs_brd",       1,  0, OPT2, ""},
  {"nom",           1,  0, OPT2, ""},
  {"google",        1,  0, OPT2, "google tile, \"x,y,z\""},
  {"rscale",        1,  0, OPT2, "reversed scale (10000 for 1:10000 map)"},
  {"dpi",           1,'d', OPT2, "resolution, dots per inch"},
  {"mag",           1,  0, OPT2, "additional magnification"},
  {"swap_y",        0,  0, OPT2, ""},

  {"htm",           1,  0, OPT3, "write html map into file"},
  {"fig",           1,'f', OPT3, "write fig-file"},
  {"map",           1,'m', OPT3, "write map-file"},
  {"draw_borders",  0,  0, OPT3, "draw map borders"},
  {"max_image",     1,  0, OPT3, "don't write images larger then this, \"x,y\", default 1000,1000"},
  {"data_marg",     1,  0, OPT3, "margins around data (works only if no geometry set)"},

  {"ks_zoom",      0,  0, OPT4, ""},
  {"google_zoom",  0,  0, OPT4, ""},
  {"google_dir",   0,  0, OPT4, ""},
  {"ks_dir",       0,  0, OPT4, ""},
  {"download",     0,  0, OPT4, ""},

  {0,0,0,0}
};

void usage(){

  const char * fname = "mapsoft_convert";
  cerr << fname << " -- convert geodata between different formats\n"
       << "Usage: "<< fname << " <options> <input files> -o <output file>\n"
       << "\n"
       << "Input files (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.fig -- mapsoft geofig format\n"
       << "  *.wpt, *.plt, *.map -- OziExplorer format\n"
       << "  *.gpx -- GPX format /partial support, tracks and waypoints/\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  *.zip -- zipped files\n"
       << " gps: --  read data from Garmin GPS via autodetected serial device\n"
       << " <character device> -- read data from Garmin GPS via serial device\n"
       << "\n"
       << "Output file (format is determined by file extension):\n"
       << "Format is determined by file extension:\n"
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
       << "\n"
       << "Options:"
  ;
  print_options(options, OPT1, cerr);
  cerr << "\nOptions for rendering images:\n";
  print_options(options, OPT3, cerr);
  cerr << "\nOptions for rendering images, geometry settings:\n";
  print_options(options, OPT2, cerr);
  cerr << "\nOptions for rendering images, google/kosmosnimki input:\n";
  print_options(options, OPT4, cerr);

  exit(1);
}

int main(int argc, char *argv[]) {
try{

  if (argc==1) usage();

  Options O = parse_options(&argc, &argv, options, OPT_ALL);
  if (O.exists("help")) usage();

  geo_data world;
  vector<string> infiles;

  while (argc>0) {
    infiles.push_back(argv[0]);
    Options O1 = parse_options(&argc, &argv, options, OPT_ALL);
    O.insert(O1.begin(), O1.end());
  }

  if (O.exists("help")) usage();

  if (!O.exists("out")){
    cerr << "no output files.\n";
    exit(1);
  }

  if (O.exists("verbose")) cerr << "Reading data...\n";

  for (vector<string>::const_iterator i = infiles.begin(); i!=infiles.end(); i++){
    io::in(*i, world, O);
  }

  if (O.exists("verbose")){
    cerr << "Map lists: " << world.maps.size()
         << ",  Waypoint lists: " << world.wpts.size()
         << ",  Tracks: " << world.trks.size() << "\n";
  }

  if (O.exists("verbose")) cerr << "Applying filters...\n";
  io::filter(world, O);
  if (O.exists("verbose")) cerr << "Writing data to " << O.get("out", string()) << "\n";
  io::out(O.get("out", string()), world, O);

} catch (const char *err){
  cerr << "ERROR: " << err << "\n";
  exit(1);
}

}

