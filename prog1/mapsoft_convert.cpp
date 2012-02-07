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

void usage(){
  const char * prog = "mapsoft_convert";

  cerr << prog << " -- convert geodata between different formats\n"
       << "  usage: " << prog << " [<global_input_options>]\\\n"
       << "         <input_file_1> [<input_options_1>] ... \\\n"
       << "         (--out|-o) <output_file> [<output_options>]\n"
       << "\n"
       << "Input files (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  *.wpt, *.plt, *.map -- OziExplorer format\n"
//       << "  .zip -- zipped OziExplorer files\n"
       << "  *.gpx -- GPX format\n"
       << "  *.fig -- geofig\n"
       << "  usb:  -- read data from GPS device via libusb.\n"
       << "  <character device name> -- read data from GPS device.\n"
       << "\n";
       << "Output files (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.gu  -- old garmin-utils format (tracks and waypoints only)\n"
       << "  *.wpt, *.plt, *.map, *.oe  -- OziExplorer format.\n"
       << "           Each track, waypoint set or map reference will be\n"
       << "           written in a separate .plt, .wpt or .map file\n"
       << "  *.zip -- zipped OziExplorer files\n"
       << "  *.kml -- Google KML format /partial support/\n"
       << "           Tracks and waypoint only, lat,lon,alt,name data only)\n"
       << "  *.kmz -- zipped kml\n"
       << "  *.gpx -- GPX format\n"
       << "  usb:  -- send data to GPS device via libusb\n"
       << "  <character device name> -- send data to GPS device\n"
       << "\n";

  cerr << "Options:\n"
       << "  --help, -h                    Show this help message\n"
       << "  --out=<file>, -o <file>       Specify output file (required)\n"
       << "  --skip=<string>, -s <string>  Skip some types of data\n"
       << "  --rescale_maps <factor>       Rescale map references\n"
       << "  --shift_maps <x>,<y>          Shift map references\n"
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
  try{

  if (argc==1) usage();

  Options O = parse_options(&argc, &argv, in_options, "out");
  Options GO(O); // global options

  geo_data V;

  /***************** input ****************/

  while (!O.exists("out")) {
    if (argc<1){
      if (O.get<int>("verbose",0))
        cout << "no output files\n";
      exit(0);
    }
    const char * ifile = argv[0];

    // parse options for this file and append global options
    O = parse_options(&argc, &argv, in_options, "out");
    O.insert(GO.begin(), GO.end());

    if (O.get<int>("verbose",0))
      cout << "reading: " << ifile  << "\n";

    geo_data V1 = vmap::read(ifile);
    io::filter(V1, O);
    V.add(V1);
  }

  /***************** output ****************/

  const char * ofile = NULL;
  if (argc<1){
    if (O.get<int>("verbose",0))
      cout << "no output files\n";
  }
  else ofile = argv[0];

  // parse output options
  O = parse_options(&argc, &argv, out_options);

  io::filter(V, O);

  if (ofile){
    if (GO.get<int>("verbose",0))
      cout << "writing to: " << ofile << "\n";
    if (!io::out(ofile, V, O)) exit(1);
  }


/*
// all these things -> io::filters ?
  io::skip(world, opts);

  if (opts.exists("shift_maps")){
    dPoint shift_maps = opts.get("shift_maps", dPoint(0,0));
    for (vector<g_map_list>::iterator ml=world.maps.begin(); ml!=world.maps.end(); ml++){
      for (vector<g_map>::iterator m=ml->begin(); m!=ml->end(); m++){
        *m+=shift_maps;
      }
    }
  }

  double rescale_maps = opts.get("rescale_maps", 1.0);
  if (rescale_maps !=1){
    for (vector<g_map_list>::iterator ml=world.maps.begin(); ml!=world.maps.end(); ml++){
      for (vector<g_map>::iterator m=ml->begin(); m!=ml->end(); m++){
        *m*=rescale_maps;
      }
    }
  }

  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }

*/

  } catch (const char *err){
    cerr << "ERROR: " << err << "\n";
    exit(1);
  }

}

