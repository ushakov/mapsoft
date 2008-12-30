#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <cstring>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "../libgeo_io/io.h"
#include "../utils/read_conf.h"

using namespace std;

void usage(const char *fname){

  cerr << "\nConvert geodata to different formats\n"
       << "Usage: "<< fname << " <in1> ... <inN> --option[=value] ... -o <out>\n\n";

  cerr << "Options:\n"
       << "  --help, -h                    Show this help message\n"
       << "  --out=<file>, -o <file>       Specify output file\n"
       << "  --skip=<string>, -s <string>  Skip some types of data\n"
       << "  --rescale_maps <factor>       Rescale map references\n"
       << "\n";

  cerr << "Output file. You must provide name for output file (-o, --out option).\n"
       << "File format is determined by its extension:\n"
       << "  .xml -- mapsoft XML-like format (all data will be written to one file)\n"
       << "  .gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  .wpt, .plt, .map, .oe  -- OziExplorer format.\n"
       << "      (Each track in single .plt file, each waypoint set in\n"
       << "       single .wpt file, each map reference in one .map file)\n"
       << "  .zip -- zipped OziExplorer format\n"
       << "  .kml -- ???\n"
       << "  .bmp,.png,.jpg -- ???\n"
       << "  .fig           -- ???\n"
       << "  .htm,.html     -- ???\n"
   << "If output file is \"usb:\" data will be sent to Garmin GPS\n"
       << "device via libusb. If output file is a character device\n"
       << "data will be sent to Garmin GPS via serial port.\n"
       << "\n";

  cerr << "Skipping data. If --skip parameter string contains letter:\n"
       << "  \"m\" -- skip maps\n"
       << "  \"w\" -- skip waypoints\n"
       << "  \"t\" -- skip tracks\n"
       << "  \"a\" -- skip ACTIVE LOG track\n"
       << "  \"o\" -- skip all but ACTIVE LOG tracks\n"
       << "\n";

  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  string outfile="";
  opts.get("out", outfile);
  if (outfile == "") usage(argv[0]);

  Options infiles; opts.get("args", infiles);

// чтение файлов

  geo_data world;
  Options::const_iterator i;

  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(i->first, world, opts);

// all these things -> io::filters ?
  io::skip(world, opts);

  double rescale_maps=1;
  opts.get("rescale_maps", rescale_maps);
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

