#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "../libgeo_io/io.h"
#include "../libgeo_io/geofig.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -f <filter1> ... -f <filterN> -O option1[=value1] ... -o <figfile>\n";
// не все так просто, надо будет написать подробнее...
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;
  list<string> infiles;
  list<string> filters;
  string figfile = "";

// разбор командной строки
  for (int i=1; i<argc; i++){ 

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage(argv[0]);

    if (strcmp(argv[i], "-o")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      figfile = argv[i];
      continue;
    }

    if (strcmp(argv[i], "-f")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      filters.push_back(argv[i]);
      continue;
    }

    if (strcmp(argv[i], "-O")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      opts.put_string(argv[i]);
      continue;
    }

    infiles.push_back(argv[i]);
  }
  if (figfile == "") usage(argv[0]);

// чтение файлов

  geo_data world;
  fig::fig_world F;
  if (!fig::read(figfile.c_str(), F)) {
    std::cerr << "File is not modified.\n";
    exit(1);
  }

  g_map ref = fig::get_ref(F);

  list<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(*i, world, opts);
  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }

  put_wpts(F, ref, world);
  put_trks(F, ref, world);

  ofstream out(figfile.c_str());
  if (!fig::write(out, F)) {
    std::cerr << "Error while writing file.\n";
    exit(1);
  }
}

