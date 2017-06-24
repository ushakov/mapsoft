#include <stdio.h>
#include <iostream>
#include <libgeo/geo_convs.h>
#include <libgeo/geo_refs.h>

int main(){

  Options o;
  o.put("lon0",39.0);
  o.put("E0",500000.0);

  g_map g = ref_google(2);

  for (int i=0; i<g.size(); i++)
    std::cerr << g[i].x << " "
              << g[i].y << " "
              << g[i].xr << " "
              << g[i].yr << "\n";
  convs::map2wgs c(g);
  dPoint p1(0,0);
  c.frw(p1);
  std::cerr << "p1: " << p1 << "\n";

  dPoint p2(256,0);
  c.frw(p2);
  std::cerr << "p2: " << p2 << "\n";

  dPoint p3(512,256);
  c.frw(p3);
  std::cerr << "p3: " << p3 << "\n";

  convs::map2map c1(g, g);

  dPoint p1a(10,10);
  c1.bck(p1a);
  std::cerr << "p1a: " << p1a << "\n";

  dPoint p2a(256,10);
  c1.bck(p2a);
  std::cerr << "p2a: " << p2a << "\n";

  dPoint p3a(512,256);
  c1.bck(p3a);
  std::cerr << "p3a: " << p3a << "\n";


}