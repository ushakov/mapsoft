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
#include "options/read_conf.h"

using namespace std;

void usage(const char *fname){

  cerr << "\nConvert geodata to different formats\n"
       << "Usage: "<< fname << " <in1> ... <inN> [--option[=value]] ... -o <out>\n\n";

  cerr << "Options:\n"
       << "  --help, -h                    Show this help message\n"
       << "  --out=<file>, -o <file>       Specify output file (required)\n"
       << "  --skip=<string>, -s <string>  Skip some types of data\n"
       << "  --rescale_maps <factor>       Rescale map references\n"
       << "  --shift_maps <x>,<y>          Shift map references\n"
       << "\n";

  cerr << "Output file. You must provide name for output file (-o, --out option).\n"
       << "File format is determined by its extension:\n"
       << "  .xml -- mapsoft XML-like format (all data will be written to one file)\n"
       << "  .gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  .wpt, .plt, .map, .oe  -- OziExplorer format.\n"
       << "      (Each track, waypoint set or map reference will be written in a\n"
       << "       separate .plt, .wpt or .map file)\n"
       << "  .zip -- zipped OziExplorer files\n"
       << "  .kml -- Google KML format (tracks and waypoint only, lat,lon,alt,name data only)\n"
       << "  .kmz -- zipped kml\n"
       << "  .tif, .tiff, .jpg, .jpeg -- raster image (map, fig, or html wrap can be made)\n"
       << "  .fig           -- ???\n"
       << "  .htm,.html     -- ???\n"
       << "If output file is \"usb:\" data will be sent to Garmin GPS\n"
       << "device via libusb. If output file is a character device\n"
       << "data will be sent to Garmin GPS via serial port.\n"
       << "\n";

/* " Options for image output
"  -s <string>\n" <<
"  --skip=<string>\n" <<
"  -O skip=<string>\n" <<
"                    -- Skipping data\n" <<
"  -g <rect>\n"
"  --geom=<rect>\n"
"  -O geom=<rect>\n"
"                    -- Geometry\n" <<
"  -p <proj>\n"
"  --proj=<proj>\n"
"  -O proj=<proj>\n"
"                    -- Projection (tmerc)\n" <<
"  -d <datum>\n"
"  --datum=<datum>\n"
"  -O datum=<datum>\n"
"                    -- Datum (pulkovo)\n" <<
*/

  cerr << "Input files. Following formats are supported:\n"
       << "  -- mapsoft XML-like format\n"
       << "  -- old garmin-utils format (for tracks and waypoints)\n"
       << "  -- OziExplorer format (for tracks, waypoints, map references)\n"
       << "  -- geofig format\n"
       << "  -- Garmin GPS device via libusb or serial port\n"
       << "     (use \"usb:\" or serial device name as a input file name)\n"
       << "\n";

  cerr << "Skipping data. If --skip parameter string contains letter:\n"
       << "  \"m\" -- skip maps\n"
       << "  \"w\" -- skip waypoints\n"
       << "  \"t\" -- skip tracks\n"
       << "  \"a\" -- skip ACTIVE LOG track\n"
       << "  \"o\" -- skip all tracks excluding ACTIVE LOG\n"
       << "\n";

  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  string outfile = opts.get("out", string());
  if (outfile == "") usage(argv[0]);

  StrVec infiles = opts.get("cmdline_args", StrVec());

// чтение файлов

  geo_data world;
  for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
    io::in(*i, world, opts);

// all these things -> io::filters ?
  io::skip(world, opts);

  dPoint shift_maps = opts.get("shift_maps", dPoint(0,0));
  if (shift_maps !=1){
    for (vector<g_map>::iterator m=world.maps.begin(); m!=world.maps.end(); m++){
      *m+=shift_maps;
    }
  }

  double rescale_maps = opts.get("rescale_maps", 1.0);
  if (rescale_maps !=1){
    for (vector<g_map>::iterator m=world.maps.begin(); m!=world.maps.end(); m++){
      *m*=rescale_maps;
    }
  }

/*  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }*/

  io::out(outfile, world, opts);
}

