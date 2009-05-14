#include "../../libgeo_io/geofig.h"
#include "../../libgeo/geo_convs.h"

// Изготовление правильного привязанного fig-файла
// на нужный диапазон координат

using namespace std;

void usage(){
    cerr << "usage: mk_gk_fig <scale, m/cm> <lon0> <x1> <x2> <y1> <y2>\n";
    cerr << "(pulkovo-42 datum, tmerc proj)\n";
    exit(0);
}

main(int argc, char** argv){

  if (argc != 7) usage();
  double scale = atof(argv[1]);
  string lon0  = argv[2];
  double x1  = atof(argv[3]);
  double x2  = atof(argv[4]);
  double y1  = atof(argv[5]);
  double y2  = atof(argv[6]);

  if (x2<x1)   swap(x2,x1);
  if (y2<y1)   swap(y2,y1);

  Options O;
  O["lon0"] = lon0;
  convs::pt2pt c("pulkovo", "tmerc", O, "wgs84", "lonlat", Options());

  g_point p1(x1,y1), p2(x1,y2), p3(x2,y2), p4(x2,y1);
  c.frw(p1); c.frw(p2); c.frw(p3); c.frw(p4); 

  double xf = (x2-x1)*fig::cm2fig/scale;
  double yf = (y2-y1)*fig::cm2fig/scale;

  g_map ref;
  ref.map_proj = Proj("tmerc");
  ref.push_back(g_refpoint(p1.x, p1.y,  0, yf));
  ref.push_back(g_refpoint(p2.x, p2.y,  0,  0));
  ref.push_back(g_refpoint(p3.x, p3.y, xf,  0));
  ref.push_back(g_refpoint(p4.x, p4.y, xf, yf));

  O["proj"] = "tmerc";
  O["datum"] = "pulkovo";
  fig::fig_world F;
  set_ref(F, ref, O);

  fig::write(cout, F);
}
