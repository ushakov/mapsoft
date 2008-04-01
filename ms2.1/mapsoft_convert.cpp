#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "libgeoio/io.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname 
       << " <in1> ... <inN> -O <opt1> ... -O <optN> -o <out>\n";
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;
  list<string> infiles;
  string outfile = "";

// optsing command line
  for (int i=1; i<argc; i++){ 

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage(argv[0]);
    if (strcmp(argv[i], "-o")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      outfile = argv[i];
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
  if (outfile == "") usage(argv[0]);

  mapsoft::geo_data world;
// reading files
  for(list<string>::const_iterator i=infiles.begin(); 
     i!=infiles.end(); i++) mapsoft::geo_read(*i, world, opts);

// applying filters
  mapsoft::geo_filter(world, opts);

// writing files
  mapsoft::geo_write(outfile, world, opts);
}

