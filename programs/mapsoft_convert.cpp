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
  list<string> infiles;

  if (!read_conf(argc, argv, opts, infiles)) usage(argv[0]);
  if (opts.exists("showhelp")) usage(argv[0]);

  string outfile="";
  opts.get("out_file", outfile);
  if (outfile == "") usage(argv[0]);


// чтение файлов

  geo_data world;
  list<string>::const_iterator i;

  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(*i, world, opts);

  io::skip(world, opts);

/*  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }*/

  io::out(outfile, world, opts);
}

