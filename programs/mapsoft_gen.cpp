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

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) exit(0);
  if (opts.exists("help")) exit(0);

  string outfile = opts.get("out", string());
  if (outfile == "") exit(0);

  StrVec infiles = opts.get("cmdline_args", StrVec());

// чтение файлов

  geo_data world;
  for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
    io::in(*i, world, opts);

// all these things -> io::filters ?
  io::skip(world, opts);

  for (vector<g_track>::iterator m=world.trks.begin(); m!=world.trks.end(); m++){
    filters::generalize(&(*m), 10, 500);
  }

/*  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }*/

  io::out(outfile, world, opts);
}

