// Изготовление закотоввки для номенклатурной карты
// в проекции Гаусса-Крюгера, СК Пулково-42.

#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"
#include "../geo_io/io.h"

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sys/stat.h>

using namespace std;

void usage(){
    cerr << "usage: make_map_nom <map name> > out.fig\n";
    cerr << "(pulkovo-42 datum, tmerc proj)\n";
    exit(0);
}


const string maps_dir = "./maps";

main(int argc, char** argv){

  if (argc != 2) usage();
  string map_name  = argv[1];

// определим диапазон карты в координатах lonlat
  Rect<double> r = filters::nom_range(map_name);
  g_line border = rect2line(r);

// определим осевой меридиан
  double lon0 = (r.TLC().x + r.TRC().x)/2;
  if (r.w > 11.9) lon0 = floor( lon0/3 ) * 3; // сдвоенные десятки
  else lon0 = floor( lon0/6.0 ) * 6 + 3;

  Options O;
  O["lon0"] = boost::lexical_cast<string>(lon0);

//масштаб карты
  double scale;
  if      (r.h < 0.33) scale = 1/50000.0;
  else if (r.h < 0.66) scale = 1/100000.0;
  else if (r.h < 1.99) scale = 1/200000.0;
  else if (r.h < 3.99) scale = 1/500000.0;
  else scale = 1/1000000.0;

  convs::pt2pt cnv(Datum("pulkovo"), Proj("lonlat"), Options(),
                   Datum("pulkovo"), Proj("tmerc"), O);

  g_point p1(r.TLC()), p2(r.TRC()), p3(r.BRC()), p4(r.BLC());

  cnv.frw(p1); cnv.frw(p2); cnv.frw(p3); cnv.frw(p4);

  p1*=fig::cm2fig*scale*100; 
  p2*=fig::cm2fig*scale*100;
  p3*=fig::cm2fig*scale*100;
  p4*=fig::cm2fig*scale*100;

  double marg = 2*fig::cm2fig;

  g_point f_min(min(p1.x, p4.x)-marg, min(p1.y, p2.y)-marg);
  g_point f_max(max(p2.x, p3.x)+marg, max(p3.y, p4.y)+marg);

  p1-=f_min; p1.y = f_max.y-f_min.y-p1.y;
  p2-=f_min; p2.y = f_max.y-f_min.y-p2.y;
  p3-=f_min; p3.y = f_max.y-f_min.y-p3.y;
  p4-=f_min; p4.y = f_max.y-f_min.y-p4.y;

  g_map ref;
  ref.map_proj = Proj("tmerc");
  ref.push_back(g_refpoint(r.TLC(), p1));
  ref.push_back(g_refpoint(r.TRC(), p2));
  ref.push_back(g_refpoint(r.BRC(), p3));
  ref.push_back(g_refpoint(r.BLC(), p4));
  ref.border.push_back(p1);
  ref.border.push_back(p2);
  ref.border.push_back(p3);
  ref.border.push_back(p4);

  convs::map2pt cnv_f(ref, Datum("wgs84"), Proj("lonlat"), Options());

  O["proj"] = "lonlat";
  O["datum"] = "pulkovo";
  fig::fig_world F;
  set_ref(F, ref, O);

  g_line brd;
  brd.push_back(r.TLC());
  brd.push_back(r.TRC());
  brd.push_back(r.BRC());
  brd.push_back(r.BLC());

  g_line brd_f = cnv_f.line_bck(brd);
  ref.border = brd_f;

  fig::fig_object brd_o = fig::make_object("2 3 0 2 15 7 36 -1 -1 0.000 0 0 -1 0 0 5");

  brd_o.push_back(Point<int>(0,0));
  brd_o.push_back(Point<int>(f_max.x-f_min.x,0));
  brd_o.push_back(Point<int>(f_max.x-f_min.x,f_max.y-f_min.y));
  brd_o.push_back(Point<int>(0, f_max.y-f_min.y));
  brd_o.push_back(Point<int>(0,0));

  F.push_back(brd_o);

  brd_o.clear();
  brd_o.insert(brd_o.end(), brd_f.begin(), brd_f.end());
  brd_o.push_back(brd_o[0]);
  F.push_back(brd_o);

  fig::fig_object o = fig::make_object("2 1 0 2 15 7 36 -1 -1 0.000 0 0 -1 0 0 5");
  int step = 2;
  for (int i = int(ceil(f_min.x*fig::fig2cm/step)); 
           i < int(floor(f_max.x*fig::fig2cm/step)); i++){
    int x = i*fig::cm2fig*step - f_min.x;
    o.clear();
    o.push_back(Point<int>(x, 0));
    o.push_back(Point<int>(x, f_max.y-f_min.y));
    list<fig::fig_object> l1, l2; l1.push_back(o);
    crop_lines1(l1, l2, brd_o, true);
    F.insert(F.end(), l1.begin(), l1.end());
  }

  for (int i = int(ceil(f_min.y*fig::fig2cm/step)); 
           i < int(floor(f_max.y*fig::fig2cm/step)); i++){
    int y = i*fig::cm2fig*step - f_min.y;
    o.clear();
    o.push_back(Point<int>(0, y));
    o.push_back(Point<int>(f_max.x-f_min.x, y));
    list<fig::fig_object> l1, l2; l1.push_back(o);
    crop_lines1(l1, l2, brd_o, true);
    F.insert(F.end(), l1.begin(), l1.end());
  }

  fig::write(cout, F);

/*

 string file = maps_dir+"/"+map_name+".fig";

  if (x2<x1)   swap(x2,x1);
  if (y2<y1)   swap(y2,y1);

  Options O;
  O["lon0"] = lon0;
  convs::pt2pt c(Datum("pulkovo"), Proj("tmerc"), O,
                 Datum("wgs84"), Proj("lonlat"), Options());

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


  
  struct stat buf;
  if (stat(file.c_str(), &buf)==0) {
    cerr << "file " << file << " exists!\n";
    exit(0);
  }
  ofstream ff(file.c_str());
  if (!ff) {
    cerr << "can't write to file " << file << "!\n";
    exit(0);
  }

  fig::write(ff, F);
*/
}
