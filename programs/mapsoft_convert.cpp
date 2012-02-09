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

  cerr << "Input files (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.fig -- mapsoft geofig format\n"
       << "  *.wpt, *.plt, *.map -- OziExplorer format\n"
       << "  *.zip -- zipped OziExplorer files\n"
       << "  *.gpx -- GPX format /partial support, tracks and waypoints/\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
//       << " usb: --  read data from Garmin GPS via libusb /doesn't work/\n"
       << " gps: --  read data from Garmin GPS via autodetected serial device\n"
       << " <character device> -- read data from Garmin GPS via serial device\n"
       << "\n";

  cerr << "Output file (format is determined by file extension):\n"
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
//       << " usb: --  send data to Garmin GPS via libusb /doesn't work/\n"
       << " gps: --  send data to Garmin GPS via autodetected serial device\n"
       << " <character device> -- send data to Garmin GPS via serial device\n"
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
try{

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

  io::filter(world, opts);
  io::out(outfile, world, opts);

} catch (const char *err){
  cerr << "ERROR: " << err << "\n";
  exit(1);
}

}

