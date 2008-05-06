#include <iostream>
#include <fstream>
#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"
#include <cmath>

using namespace std;

// подстройка привязки карты
// ищутся партые ломаные 
// глубина 2 -- правильное положение
// глубина 3 -- неправильное положение


main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " in.fig out.fig\n";
    exit(0);
  }

  fig::fig_world W1  = fig::read(argv[1]);
  ofstream out(argv[2]);

  g_map map = fig::get_ref(W1);
  convs::map2pt cnv(map, "wgs84", "lonlat");

  rem_ref(W1);
  g_map new_map;

  fig::fig_world::iterator i,j;
  for (i=W1.begin(); i!=W1.end(); i++){
    if (i->type!=2) continue;
    if (i->size()<2) continue;
    bool is_copy = false;
    Point<int> d;
    for (j=W1.begin(), j++; j!=W1.end(); j++){
      if (j->size()!=i->size()) continue;
      if ((i->depth !=2) || (j->depth!=3)) continue;

      d = (*i)[0] - (*j)[0];
      is_copy = true;
      for (int k = 1; k<i->size(); k++){
        if ((*i)[k] - (*j)[k] != d) {is_copy = false; continue;}
      }
      if (is_copy) {
	std::cerr << d <<"\n";
        g_point p1 = g_point((*i)[0]);
        g_point p2 = p1+g_point(d);

        g_refpoint rp(p1.x, p1.y, p2.x, p2.y);
        cnv.frw(rp);
        new_map.push_back(rp);
        continue; 
      }
    }
  } 
  set_ref(W1, new_map, Options());
  
  fig::write(out, W1);
}
