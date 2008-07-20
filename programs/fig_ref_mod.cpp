#include <fstream>
#include <cstring>
#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"

// Модификация привязки fig-файла.
// Меняются только координаты в точках привязки!

// fig_ref_mod -r 10 in_file out_file  -- повернуть вокруг центра на 10 градусов
// fig_ref_mod -x -5 in_file out_file  -- сместить по x на -5 m
// fig_ref_mod -y 10 in_file out_file  -- сместить по y на 10 m

void usage(char *name){
      std::cerr << "usage: \n" << 
        name << " -r <degree> <referenced fig>\n" << 
        name << " -x <m> <referenced fig>\n" << 
        name << " -y <m> <referenced fig>\n";
      exit(0);
}

main(int argc, char **argv){
  if (argc<4) usage(argv[0]);
  double value = atof(argv[2]);

  char action = 0;
  if (strcmp(argv[1], "-r")==0) action = 'r';
  if (strcmp(argv[1], "-x")==0) action = 'x';
  if (strcmp(argv[1], "-y")==0) action = 'y';
  if (action == 0) usage(argv[0]);


  fig::fig_world W;
  if (!fig::read(argv[3], W)) {
    std::cerr << "File is not modified.\n";
    exit(1);
  }
  g_map map = fig::get_ref(W);
  fig::rem_ref(W);

  g_rect  range  = map.range();
  g_point center = (range.TLC() + range.BRC())/2.0;

  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(), Datum("wgs84"), Proj("tmerc"), Options());
  cnv.frw(center);

  if (action == 'r'){
    double c = cos(value*M_PI/180.0);
    double s = sin(value*M_PI/180.0);

    for (g_map::iterator p=map.begin(); p!=map.end(); p++){
      cnv.frw(*p);
      double x = p->x - center.x, y = p->y - center.y;
      p->x = x*c - y*s + center.x;
      p->y = x*s + y*c + center.y;
      cnv.bck(*p);
    }
  }
  if (action == 'x'){
    for (g_map::iterator p=map.begin(); p!=map.end(); p++){
      cnv.frw(*p);
      p->x += value;
      cnv.bck(*p);
    }
  }
  if (action == 'y'){
    for (g_map::iterator p=map.begin(); p!=map.end(); p++){
      cnv.frw(*p);
      p->y += value;
      cnv.bck(*p);
    }
  }

  fig::set_ref(W, map, Options());
  std::ofstream f(argv[3]);
  fig::write(f, W);
}
