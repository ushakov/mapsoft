#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include <string>
#include <cstring>

#include <sys/stat.h>
#include <math.h>

#include "../core/libgeo_io/io.h"
#include "../core/libgeo/geo_convs.h"
#include "../core/libgeo/geo_nom.h"


using namespace std;
using namespace boost::spirit;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <infile> -o <outfile>\n";
  exit(0);
}

main(int argc, char **argv){

  string infile, outfile;

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

  if ((infile=="")||(outfile=="")) usage(argv[0]);

// Читаем файл, содержащий информации о привязке карт
// N37-001.jpg  51 216 3726 216 3741 4789  31 4591  Руза, 1982-1983
// (номенклатура-имя файла, координаты 4 углов по часовой стрелке, название)

#define BUFSIZE 2048

  Options opts;
  geo_data world;
  g_map map;
 
  fstream in(infile.c_str());

  do {
    string key, comm, ext;
    char buf[BUFSIZE];
    int n[8];
    
    in.getline(buf, BUFSIZE);

    if (parse(buf, ch_p('#') >> *anychar_p).full) continue;
    if (parse(buf, *space_p).full) continue;

    if (!parse(buf, (+(graph_p - '.'))[assign_a(key)] >> 
	ch_p('.') >> (+graph_p)[assign_a(ext)] >> +space_p >>
        int_p[assign_a(n[0])] >> +space_p >> 
        int_p[assign_a(n[1])] >> +space_p >>
        int_p[assign_a(n[2])] >> +space_p >> 
	int_p[assign_a(n[3])] >> +space_p >>
        int_p[assign_a(n[4])] >> +space_p >> 
	int_p[assign_a(n[5])] >> +space_p >>
        int_p[assign_a(n[6])] >> +space_p >> 
	int_p[assign_a(n[7])] >> +space_p >>
        (*anychar_p)[assign_a(comm)]).full){
      std::cerr << "strange line: " << buf << "\n";
      continue;
    }
    std::cout << key << " " << ext << " " << comm << "\n";
    dRect r = convs::nom_range(key);
    if (r.empty()) {
      std::cerr << "bad key: " << key << "\n";
      continue;
    }

    g_map map;
    map.file = key + '.' + ext;
    map.comm = key + ' ' + comm;
    map.map_proj=Proj("tmerc");

    g_point p1 = r.TLC();
    g_point p2 = r.BRC();

      std::cerr << "key: " << key << '\n';
      std::cerr << "lon: " << p1.x << " - "<< p2.x << '\n';
      std::cerr << "lat: " << p1.y << " - "<< p2.y << '\n';

    convs::ll2wgs dc(Datum("Pulkovo 1942"));
    dc.frw(p1);
    dc.frw(p2);

    map.push_back(g_refpoint(p1.x,p2.y,n[0],n[1]));
    map.push_back(g_refpoint(p2.x,p2.y,n[2],n[3]));
    map.push_back(g_refpoint(p2.x,p1.y,n[4],n[5]));
    map.push_back(g_refpoint(p1.x,p1.y,n[6],n[7]));
    world.maps.push_back(map);

  } while (!in.eof());
  filters::map_nom_brd(world);
  io::out(outfile, world, opts);
}


