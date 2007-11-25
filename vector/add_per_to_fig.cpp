// Добавть в привязанный fig-файл точки (WPT) с перевалами и вершинами
// разные категории перевалов symb=1..7, вершины symb=8


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cmath>

#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"
#include "../geo_io/io.h"

using namespace std;

void usage(const char *fname){
  cerr << "usage: " << fname << " <wpt-file> <in fig-file> <out fig-file>\n";
  exit(0);
}

main(int argc, char **argv){

  Options opts;
  string wptfile, figfile, outfig;

// разбор командной строки
  if (argc!=4) usage(argv[0]);
  wptfile = argv[1];
  figfile = argv[2];
  outfig = argv[3];

  geo_data world;
  io::in(wptfile, world, Options());

  fig::fig_world fig = fig::read(figfile.c_str());
  g_map ref=get_ref(fig);

  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

  for (int j = 0; j< world.wpts.size(); j++){
    for (int i = 0; i< world.wpts[j].size(); i++){
      g_waypoint pt = world.wpts[j][i]; 

      if ((pt.symb<1)||(pt.symb>8)) continue;
      cnv.bck(pt);
      int c1=7,  c2=15;  // цвета внутр. и нар. крестиков
      int d1=20, d2=21; // глубина 
      int w1, w2;       // толщина линий
      int s;            // размах
      switch (pt.symb){
        case 1: w1=5; w2=4; s=8; break; // н/к
        case 2: w1=4; w2=3; s=6; break;
        case 3: w1=4; w2=3; s=6; break;
        case 4: w1=3; w2=3; s=4; break;
        case 5: w1=3; w2=3; s=4; break;
        case 6: w1=2; w2=2; s=3; break;
        case 7: w1=2; w2=2; s=3; break; // 3Б
        case 9: w1=2; w2=2; s=3; c2=0; break; // неизв.
        case 8: c2 = 24; w2 = 4; s=2; break; // V
      }
      s*=12;

      Point<double> p1(pt.x, pt.y), p2=p1;
      Point<double> v1, v2;
      if ( fig.nearest_pt(v2, p2, "2 * * * * * 89 * * * * * * * * *") <
           fig.nearest_pt(v1, p1, "3 * * * * * 89 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v1=Point<double>(v1.x-v1.y,v1.y+v1.x)/sqrt(2.0); 
      v2=Point<double>(-v1.y,v1.x);


      fig::fig_object o = fig::make_object("2 1 0 2 17 7 0 -1 -1 0.000 0 1 7 0 0 *");
      if ((pt.symb >0)&&(pt.symb <8)){
        o.thickness = w2; o.pen_color = c2; o.depth = d2;
        o.push_back(Point<int>(int(p1.x), int(p1.y)) - Point<int>(int(s*v1.x),int(s*v1.y)));
        o.push_back(Point<int>(int(p1.x), int(p1.y)) + Point<int>(int(s*v1.x),int(s*v1.y)));
        fig.push_back(o); 
        o.clear();
        o.push_back(Point<int>(int(p1.x), int(p1.y)) - Point<int>(int(s*v2.x),int(s*v2.y)));
        o.push_back(Point<int>(int(p1.x), int(p1.y)) + Point<int>(int(s*v2.x),int(s*v2.y)));
        fig.push_back(o); 
        o.clear();
      }
      if ((pt.symb==3)||(pt.symb==5)||(pt.symb==7)){
        o.thickness = w1; o.pen_color = c1; o.depth = d1;
        o.push_back(Point<int>(int(p1.x), int(p1.y)));
        fig.push_back(o);
      }
      if (pt.symb ==8 ){
        o.thickness = w2; o.pen_color = c2; o.depth = d1;
        o.push_back(Point<int>(int(p1.x), int(p1.y)));
        fig.push_back(o);
      }

      fig::fig_object t = fig::make_object("4 0 0 55 -1 18 7 0.0000 4");
      t.pen_color = c2; t.depth = d2;
      t.text=pt.name.c_str();
      t.push_back(Point<int>(int(p1.x), int(p1.y)) + Point<int>(s,-s));
      fig.push_back(t);
    }
  }
  ofstream out(outfig.c_str());
  fig::write(out, fig);
}
