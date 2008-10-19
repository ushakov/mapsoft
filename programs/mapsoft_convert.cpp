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
  cerr << "Usage: "<< fname << " <in1> ... <inN> -O option1[=value1] ... -o <out>\n";

cerr <<
""<<
"  -O skip=<string>\n" <<
"  -s <string>\n" <<
"                    -- Skipping data\n" <<
"\n" <<
"  If <string> contains letter:\n" <<
"    \"m\" - skip maps\n" <<
"    \"w\" - skip waypoints\n" <<
"    \"t\" - skip tracks\n" <<
"    \"a\" - skip ACTIVE LOG track\n" <<
"    \"o\" - skip all but ACTIVE LOG tracks\n" <<
"\n";

// не все так просто, надо будет написать подробнее...
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  string outfile="";
  opts.get("out_file", outfile);
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

