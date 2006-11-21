#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "../geo_io/io.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -o <out>\n";
// не все так просто, надо будет написать подробнее...
  exit(0);
}

int main(int argc, char *argv[]) {

  list<string> infiles;
  string outfile = "";

// разбор командной строки
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

    infiles.push_back(argv[i]);
  }
  if (outfile == "") usage(argv[0]);

// чтение файлов

  Options opts;
  geo_data world;
  list<string>::const_iterator b=infiles.begin(), e=infiles.end(), i;
  for(i=b; i!=e; i++){
    io::in(*i, world, opts);
  }
  io::out(outfile, world, opts);
}

