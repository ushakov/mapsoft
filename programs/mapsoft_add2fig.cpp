#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "libgeo_io/io.h"
#include "libgeo_io/geofig.h"
#include "utils/read_conf.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -o <figfile>\n";
// не все так просто, надо будет написать подробнее...
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  string figfile = opts.get("out", string());
  if (figfile == "") usage(argv[0]);




  list<string> filters;

// чтение файлов

  geo_data world;
  fig::fig_world F;
  if (!fig::read(figfile.c_str(), F)) {
    std::cerr << "File is not modified. Exiting...\n";
    exit(1);
  }

  g_map ref = fig::get_ref(F);

  StrVec infiles = opts.get("cmdline_args", StrVec());
  for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
    io::in(*i, world, opts);

/*  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }*/

  io::skip(world, opts);

  put_wpts(F, ref, world);
  put_trks(F, ref, world);

  exit (!fig::write(figfile, F));
}

