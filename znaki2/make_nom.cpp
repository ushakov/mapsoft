// Изготовление закотоввки для номенклатурной карты
// в проекции Гаусса-Крюгера, СК Пулково-42.

#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"
#include "../geo_io/io.h"

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>
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
  Rect<double> r0 = filters::nom_range(map_name);

// определим осевой меридиан
  double lon0 = (r0.TLC().x + r0.TRC().x)/2;
  if (r0.w > 11.9) lon0 = floor( lon0/3 ) * 3; // сдвоенные десятки
  else lon0 = floor( lon0/6.0 ) * 6 + 3;

  Options O;
  O["lon0"] = boost::lexical_cast<string>(lon0);

//масштаб карты
  double scale;
  if      (r0.h < 0.33) scale = 1/50000.0;
  else if (r0.h < 0.66) scale = 1/100000.0;
  else if (r0.h < 1.99) scale = 1/200000.0;
  else if (r0.h < 3.99) scale = 1/500000.0;
  else scale = 1/1000000.0;

  // Наш прямоугольник в СК Пулково!

  convs::pt2pt c0(Datum("pulkovo"), Proj("lonlat"), Options(),
                  Datum("wgs84"),   Proj("lonlat"), Options());
  g_point p01(r0.TLC()), p02(r0.BRC());
  c0.frw(p01); c0.frw(p02);
  Rect<double> r = Rect<double>(p01, p02);

  g_line border_ll = rect2line(r);
  border_ll.push_back(p01);


  convs::pt2pt cnv(Datum("pulkovo"), Proj("tmerc"), O, 
                   Datum("wgs84"), Proj("lonlat"), Options());

//  g_line border_fig = cnv.line_frw(border_ll) * fig::cm2fig*scale*100;
  
  g_point p1(r.TLC()), p2(r.TRC()), p3(r.BRC()), p4(r.BLC());

  cnv.bck(p1); cnv.bck(p2); cnv.bck(p3); cnv.bck(p4);

  double m2fig = fig::cm2fig*scale*100;
  p1*=m2fig; 
  p2*=m2fig;
  p3*=m2fig;
  p4*=m2fig;

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

  convs::map2pt cnv_f(ref, Datum("wgs84"), Proj("lonlat"), O);

  O["proj"] = "lonlat";
  O["datum"] = "pulkovo";
  fig::fig_world F;
  set_ref(F, ref, O);

  ref.border = cnv_f.line_bck(border_ll);

//  ref.border = cnv.line_bck(border_ll, 1/m2fig) * m2fig;

  fig::fig_object brd_o = fig::make_object("2 3 0 2 0 7 31 -1 -1 0.000 0 0 -1 0 0 5");

  brd_o.push_back(Point<int>(0,0));
  brd_o.push_back(Point<int>(f_max.x-f_min.x,0));
  brd_o.push_back(Point<int>(f_max.x-f_min.x,f_max.y-f_min.y));
  brd_o.push_back(Point<int>(0, f_max.y-f_min.y));
  brd_o.push_back(Point<int>(0,0));

  F.push_back(brd_o);

  brd_o.clear();
  brd_o.comment.push_back("BRD "+map_name);
  brd_o.insert(brd_o.end(), ref.border.begin(), ref.border.end());
  brd_o.push_back(brd_o[0]);
  F.push_back(brd_o);

  fig::fig_object o = fig::make_object("2 1 0 2 0 7 36 -1 -1 0.000 0 0 -1 0 0 5");
  fig::fig_object t = fig::make_object("4 1 0 31 -1 18 10 0.0000 4");
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
    if (l1.size()>0){
      t.clear();
      t.text = boost::lexical_cast<std::string>(int(2*i/scale/100000.0));
      t.push_back(l1.front()[0] + Point<int>(0, -0.2*fig::cm2fig));
      F.push_back(t);
      t.clear();
      t.push_back(l1.back()[l1.back().size()-1] + Point<int>(0, 0.6*fig::cm2fig));
      F.push_back(t);
    }
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
    if (l1.size()>0){
      t.clear(); t.sub_type = 2;
      t.text = boost::lexical_cast<std::string>(int(2*i/scale/100000.0));
      t.push_back(l1.front()[0] + Point<int>(-0.2*fig::cm2fig,0.2*fig::cm2fig));
      F.push_back(t);
      t.clear(); t.sub_type = 0;
      t.push_back(l1.back()[l1.back().size()-1] + Point<int>(0.2*fig::cm2fig,0.2*fig::cm2fig));
      F.push_back(t);
    }
  }

  ostringstream s; 
  for (int i = 0; i<4; i++){
    
    Point<double> p(ref[i]);
    Point<int> pr(ref[i].xr, ref[i].yr);
    c0.bck(p); // в Пулково
    s.str(""); t.clear(); t.sub_type = 2-(((i+1)/2)%2) *2; t.font_size = 8;
    int deg = int(floor(p.y+1/120.0));
    int min = int(floor(p.y*60+1/2.0))-deg*60;
    s << deg << "*"
      << setw(2) << setfill('0') << min;
    t.text = s.str();
    t.push_back(pr-Point<int>((t.sub_type-1) * 0.2*fig::cm2fig, 0));
    F.push_back(t);

    s.str(""); t.clear(); t.sub_type = 1;
    deg = int(floor(p.x+1/120.0));
    min = int(floor(p.x*60+1/2.0))-deg*60;
    s << deg << "*" 
      << setw(2) << setfill('0') << min;
    t.text = s.str();
    t.push_back(pr+Point<int>(0, (1-(i/2)*2)* 0.3*fig::cm2fig));
    F.push_back(t);
  }


  t.font_size=12;
  t.sub_type = 2;
  t.angle = M_PI/2;
  t.text  = "0000-00-00";
  t.comment.clear(); t.comment.push_back("CURRENT DATE");
  t.clear();
  t.push_back(Point<int>(0.5*fig::cm2fig, 6.0*fig::cm2fig));
  F.push_back(t);
  t.comment.clear();

  t.font_size=20;
  t.sub_type = 0;
  t.angle = 0;
  t.text  = map_name;
  t.clear();
  t.push_back(Point<int>(1.8*fig::cm2fig, 1.0*fig::cm2fig));
  F.push_back(t);

  t.sub_type = 1; t.text="z"; t.clear(); 
  t.push_back(Point<int>(0.8*fig::cm2fig, 1.0*fig::cm2fig));
  F.push_back(t);


  t.type = 1;
  t.radius_x = t.radius_y = 0.3*fig::cm2fig;
  t.center_x = t[0].x;
  t.center_y = t[0].y - 0.2*fig::cm2fig;
  F.push_back(t);

  fig::write(cout, F);
}
