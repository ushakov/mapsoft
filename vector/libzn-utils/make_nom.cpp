// Изготовление закотоввки для номенклатурной карты
// в проекции Гаусса-Крюгера, СК Пулково-42.

#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "geo_io/geofig.h"
#include "geo_io/io_oe.h"

#include "2d/line_utils.h"
#include "options/m_getopt.h"
#include "2d/line_polycrop.h"

#include "loaders/image_r.h" // определение размеров картинки (image_r::size)

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

using namespace std;

void usage(){
    cerr << "usage: make_nom [-m <magnification>] <map name> > out.fig\n";
    cerr << "       make_nom [-m <magnification>] <png map name> <dpi> > out.map\n";
    cerr << "(pulkovo-42 datum, tmerc proj)\n";
    exit(0);
}

static struct option opts[] = {
  {"magnification",           1, 0, 'm'},
  {0,0,0,0}
};

const string maps_dir = "./maps";

main(int argc, char** argv){

  Options cmdline_opts = parse_options(&argc, &argv, opts);
  double magnification = cmdline_opts.get<double>("magnification", 1.0);

  bool dofig;

  if (argc == 1) dofig = true;
  else if (argc == 2) dofig = false;
  else usage();
  string map_name  = argv[0];
  int dpi;


// определим диапазон карты в координатах lonlat
  dRect r0 = convs::nom_to_range(map_name);
  if (r0.empty()){
    std::cerr << "Bad name: " << map_name << "\n";
    exit(1);
  }

// определим осевой меридиан
  double lon0 = (r0.TLC().x + r0.TRC().x)/2;
  if (r0.w > 11.9) lon0 = floor( lon0/3 ) * 3; // сдвоенные десятки
  else lon0 = convs::lon2lon0(lon0);

  Options O;
  O.put<double>("lon0", lon0);

//масштаб карты
  double scale;
  if      (r0.h < 0.33) scale = 1/50000.0;
  else if (r0.h < 0.66) scale = 1/100000.0;
  else if (r0.h < 1.99) scale = 1/200000.0;
  else if (r0.h < 3.99) scale = 1/500000.0;
  else scale = 1/1000000.0;
  scale*=magnification;

  // Наш прямоугольник в СК Пулково!

  convs::pt2pt c0(Datum("pulkovo"), Proj("lonlat"), Options(),Datum("wgs84"),Proj("lonlat"), Options());
  dPoint p01(r0.TLC()), p02(r0.BRC());
  c0.frw(p01); c0.frw(p02);
  dRect r = dRect(p01, p02);

  dLine border_ll = rect2line(r);


  convs::pt2pt cnv(Datum("pulkovo"), Proj("tmerc"), O, Datum("wgs84"), Proj("lonlat"), Options());

//  dLine border_fig = cnv.line_frw(border_ll) * fig::cm2fig*scale*100;

  dPoint p1(r.TLC()), p2(r.TRC()), p3(r.BRC()), p4(r.BLC());

  cnv.bck(p1); cnv.bck(p2); cnv.bck(p3); cnv.bck(p4);


  double cm2pt = fig::cm2fig;

  if (!dofig){
    double dpi = atof(argv[1]);
    cm2pt = dpi/2.54;
  }

  double m2pt  = cm2pt*scale*100;


  p1*=m2pt;
  p2*=m2pt;
  p3*=m2pt;
  p4*=m2pt;

  double marg = 2*cm2pt;

  dPoint f_min(min(p1.x, p4.x)-marg, min(p1.y, p2.y)-marg);
  dPoint f_max(max(p2.x, p3.x)+marg, max(p3.y, p4.y)+marg);

  p1-=f_min; p1.y = f_max.y-f_min.y-p1.y;
  p2-=f_min; p2.y = f_max.y-f_min.y-p2.y;
  p3-=f_min; p3.y = f_max.y-f_min.y-p3.y;
  p4-=f_min; p4.y = f_max.y-f_min.y-p4.y;

  g_map ref;
  ref.map_proj = Proj("tmerc");
  ref.file = map_name;
  ref.push_back(g_refpoint(r.TLC(), p1));
  ref.push_back(g_refpoint(r.TRC(), p2));
  ref.push_back(g_refpoint(r.BRC(), p3));
  ref.push_back(g_refpoint(r.BLC(), p4));
  ref.border.push_back(p1);
  ref.border.push_back(p2);
  ref.border.push_back(p3);
  ref.border.push_back(p4);

  convs::map2pt cnv_f(ref, Datum("wgs84"), Proj("lonlat"), O);
  ref.border = cnv_f.line_bck(border_ll);

  if (dofig){

    O["proj"] = "lonlat";
    O["datum"] = "pulkovo";
    fig::fig_world F;
    set_ref(F, ref, O);

    fig::fig_object brd_o = fig::make_object("2 3 0 1 0 7 31 -1 -1 0.000 0 0 -1 0 0 5");

    brd_o.push_back(iPoint(0,0));
    brd_o.push_back(iPoint(f_max.x-f_min.x,0));
    brd_o.push_back(iPoint(f_max.x-f_min.x,f_max.y-f_min.y));
    brd_o.push_back(iPoint(0, f_max.y-f_min.y));
    brd_o.push_back(iPoint(0,0));

    F.push_back(brd_o);

    fig::fig_object brd_w = fig::make_object("2 3 0 0 0 7 32 -1 20 0.000 0 0 -1 0 0 *");
    brd_w.insert(brd_w.end(), brd_o.begin(), brd_o.end());

    brd_o.clear();
    brd_o.comment.push_back("BRD "+map_name);
    brd_o.insert(brd_o.end(), ref.border.begin(), ref.border.end());
    brd_o.push_back(brd_o[0]);
    F.push_back(brd_o);

    brd_w.insert(brd_w.end(), brd_o.begin(), brd_o.end());
    F.push_back(brd_w);

    fig::fig_object o = fig::make_object("2 1 0 1 0 7 36 -1 -1 0.000 0 0 -1 0 0 5");
    fig::fig_object t = fig::make_object("4 1 0 31 -1 18 10 0.0000 4");
    o.comment.push_back("[grid]");
    t.comment.push_back("[grid labels]");
    int step = 2 * magnification;

    for (int i = int(ceil(f_min.x*fig::fig2cm/step));
             i < int(floor(f_max.x*fig::fig2cm/step)); i++){
      int x = i*cm2pt*step - f_min.x;
      o.clear();
      o.push_back(iPoint(x, 0));
      o.push_back(iPoint(x, f_max.y-f_min.y));
      vector<fig::fig_object> l1, l2; l1.push_back(o);
      crop_lines(l1, l2, brd_o, true);
      F.insert(F.end(), l1.begin(), l1.end());
      if (l1.size()>0){
        t.clear();
        t.text = boost::lexical_cast<std::string>(int(floor(2*i/scale/100000.0+0.5)));
        t.push_back(l1.front()[0] + iPoint(0, -0.3*cm2pt));
        F.push_back(t);
        t.clear();
        t.push_back(l1.back()[l1.back().size()-1] + iPoint(0, 0.7*cm2pt));
        F.push_back(t);
      }
    }

    for (int i = int(ceil(f_min.y*fig::fig2cm/step));
             i < int(floor(f_max.y*fig::fig2cm/step)); i++){
      int y = f_max.y - i*cm2pt*step;
      o.clear();
      o.push_back(iPoint(0, y));
      o.push_back(iPoint(f_max.x-f_min.x, y));
      vector<fig::fig_object> l1, l2; l1.push_back(o);
      crop_lines(l1, l2, brd_o, true);
      F.insert(F.end(), l1.begin(), l1.end());
      if (l1.size()>0){
        t.clear(); t.sub_type = 2;
        t.text = boost::lexical_cast<std::string>(int(floor(2*i/scale/100000.0 + 0.5)));
        t.push_back(l1.front()[0] + iPoint(-0.2*cm2pt, 0.2*cm2pt));
        F.push_back(t);
        t.clear(); t.sub_type = 0;
        t.push_back(l1.back()[l1.back().size()-1] + iPoint(0.2*cm2pt, 0.2*cm2pt));
        F.push_back(t);
      }
    }

    ostringstream s;
    for (int i = 0; i<4; i++){

      dPoint p(ref[i]);
      iPoint pr(ref[i].xr, ref[i].yr);
      c0.bck(p); // в Пулково

      s.str(""); t.clear(); t.sub_type = 2-(((i+1)/2)%2) *2; t.font_size = 8;
      int deg = int(floor(p.y+1/120.0));
      int min = int(floor(p.y*60+1/2.0))-deg*60;
      s << deg << "*"
        << setw(2) << setfill('0') << min;
      t.text = s.str();
      t.push_back(pr-iPoint((t.sub_type-1) * 0.1*cm2pt, -(i/2)*0.2*cm2pt));
      F.push_back(t);

      s.str(""); t.clear(); t.sub_type = 1;
      deg = int(floor(p.x+1/120.0));
      min = int(floor(p.x*60+1/2.0))-deg*60;
      s << deg << "*"
        << setw(2) << setfill('0') << min;
      t.text = s.str();
      t.push_back(pr+iPoint(0, (1.7-(i/2)*2)* 0.2*cm2pt));
      F.push_back(t);
    }


    t.font_size=12;
    t.sub_type = 2;
    t.angle = M_PI/2;
    t.text  = "0000-00-00";
    t.comment.clear(); t.comment.push_back("CURRENT DATE");
    t.clear();
    t.push_back(iPoint(0.5*cm2pt, 6.0*cm2pt));
    F.push_back(t);
    t.comment.clear();

    t.font_size=20;
    t.sub_type = 0;
    t.angle = 0;
    t.text  = map_name;
    t.clear();
    t.push_back(iPoint(1.8*cm2pt, 1.0*cm2pt));
    F.push_back(t);

    t.sub_type = 1; t.text="z"; t.clear();
    t.push_back(iPoint(0.8*cm2pt, 1.0*cm2pt));
    F.push_back(t);


    t.type = 1;
    t.radius_x = t.radius_y = 0.3*cm2pt;
    t.center_x = t[0].x;
    t.center_y = t[0].y - 0.2*cm2pt;
    F.push_back(t);

    fig::write(cout, F);
  } else {
    cerr << "writing map file\n";
    dPoint wh = image_r::size(map_name.c_str());
    ref -= (f_max-f_min-wh)/2.0;

    try {oe::write_map_file("out.map", ref);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}

  }
}
