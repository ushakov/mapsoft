#include "fig.h"
#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"
int main(){
  fig::fig_world W;
  fig::read("test.fig", W);
  g_map map = fig::get_ref(W);
  std::cerr << "pt2pt\n";
  convs::pt2pt c1("wgs84", "lonlat", Options(), "pulkovo", "tmerc", Options());
  std::cerr << "map2pt\n";
  convs::map2pt c2(map, "pulkovo", "tmerc");
}