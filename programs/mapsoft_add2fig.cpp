#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "../geo_io/io.h"
#include "../geo_io/geofig.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -f <filter1> ... -f <filterN> -O option1[=value1] ... -i <infig> -o <outfig>\n";
// �� ��� ��� ������, ���� ����� �������� ���������...
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;
  list<string> infiles;
  list<string> filters;
  string outfile = "";
  string infile = "";

// ������ ��������� ������
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

    if (strcmp(argv[i], "-i")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      infile = argv[i];
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
  if (outfile == "") usage(argv[0]);
  if (infile == "") usage(argv[0]);

// ������ ������

  geo_data world;
  fig::fig_world F = fig::read(infile.c_str());
  g_map ref = fig::get_ref(F);

  list<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(*i, world, opts);
  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }

  put_wpts(F, ref, world);
  put_trks(F, ref, world);

  ofstream out(outfile.c_str());
  fig::write(out, F);
}
