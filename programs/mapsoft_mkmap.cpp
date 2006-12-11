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
  cerr << "Usage: "<< fname << " <map picture> -o <out>\n";
  exit(0);
}

int main(int argc, char *argv[]) {

  string key, infile, outfile;

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
    infile = argv[i];
  }
  if (outfile == "") usage(argv[0]);
  if (infile == "") usage(argv[0]);

  Options opts;
  geo_data world;
  g_map map;

  Rect<double> r;
  do {
    std::cout << "Map key and comment: ";
    std::cin >> key >> '\n';

    r = filters::nom_range(key);
    if (r.empty()) std::cout << "can't parse!\n";
  } while (!r.empty());

  map.file=infile;
  map.comm=key;

  double lon1 = r.x;
  double lat1 = r.y;
  double lon2 = lon1 + r.w;
  double lat2 = lat1 + r.h;

  // привязка

  double x,y;
  std::cout << "top-left corner: "; std::cin >> x >> y;
  map.points.push_back(g_refpoint(lon1,lat2,x,y));
  std::cout << "top-right corner: "; std::cin >> x >> y;
  map.points.push_back(g_refpoint(lon2,lat2,x,y));
  std::cout << "bottom-right corner: "; std::cin >> x >> y;
  map.points.push_back(g_refpoint(lon2,lat1,x,y));
  std::cout << "bottom-left corner: "; std::cin >> x >> y;
  map.points.push_back(g_refpoint(lon1,lat1,x,y));

  world.maps.push_back(map);
  filters::map_nom_brd(world);

  io::out(outfile, world, opts);
}

