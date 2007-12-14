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

  // Наш прямоугольник в СК Пулково!

  convs::pt2pt c0(Datum("pulkovo"), Proj("lonlat"), Options(),
                  Datum("wgs84"),   Proj("lonlat"), Options());
  g_point p01(r.TLC()), p02(r.BRC());
  c0.frw(p01); c0.frw(p02);
  r = Rect<double>(p01, p02);

  g_line border = rect2line(r);

  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
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
  brd_o.comment.push_back("BRD "+map_name);
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
    crop_lines(l1, l2, brd_o, true);
    F.insert(F.end(), l1.begin(), l1.end());
  }

  for (int i = int(ceil(f_min.y*fig::fig2cm/step)); 
           i < int(floor(f_max.y*fig::fig2cm/step)); i++){
    int y = f_max.y - i*fig::cm2fig*step;
    o.clear();
    o.push_back(Point<int>(0, y));
    o.push_back(Point<int>(f_max.x-f_min.x, y));
    list<fig::fig_object> l1, l2; l1.push_back(o);
    crop_lines(l1, l2, brd_o, true);
    F.insert(F.end(), l1.begin(), l1.end());
  }

  o.type = 4; o.sub_type=0; o.font=18; o.font_size=20; o.font_flags=4;
  o.text = map_name;
  o.clear();
  o.push_back(Point<int>(0.5*fig::cm2fig, 1.2*fig::cm2fig));
  F.push_back(o);

  o.sub_type = 1; o.text="z"; o.clear(); 
  o.push_back(Point<int>(f_max.x-f_min.x-1*fig::cm2fig, f_max.y-f_min.y-1*fig::cm2fig));
  F.push_back(o);

  o.type = 1;
  o.radius_x = o.radius_y = 0.3*fig::cm2fig;
  o.center_x = o[0].x;
  o.center_y = o[0].y - 0.2*fig::cm2fig;
  F.push_back(o);

  fig::write(cout, F);
}
