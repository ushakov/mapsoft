#include <iostream>
#include <fstream>
#include "../geo_io/fig.h"
#include <cmath>
#include "zn.h"
#include "line_dist.h"

using namespace std;
using namespace fig;

// Найти ближайшую к точке pt линию типа type.
// В pt запихать эту ближайшую точку, в vec - едиичный вектор направления линии,
// вернуть расстояние. 
// Поиск происходит на расстоянии не более maxdist (xfig units)
double nearest_line(list<fig_object> fig, int type, Point<double> & vec, Point<double> & pt, double maxdist=100){

  Point<double> minp(pt),minvec(1,0);
  double minl=maxdist;

  for (list<fig_object>::const_iterator i  = fig.begin(); i != fig.end(); i++){

    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key k = zn::get_key(*i);
    if (k.type!=type) continue;

    for (int j=1; j<i->size(); j++){
      Point<double> p1((*i)[j-1]);
      Point<double> p2((*i)[j]);

      double  ll = pdist(p1,p2);
      if (ll==0) continue;
      Point<double> vec = (p2-p1)/ll;

      double ls = pdist(pt,p1);
      double le = pdist(pt,p2);

      if (ls<minl){ minl=ls; minp=p1; minvec=vec; }
      if (le<minl){ minl=le; minp=p2; minvec=vec; }

      double prl = pscal(pt-p1, vec);

      if ((prl>=0)&&(prl<=ll)) { // проекция попала на отрезок
        Point<double> pc = p1 + vec * prl;
        double lc=pdist(pt,pc);
        if (lc<minl) { minl=lc; minp=pc; minvec=vec; }
      }
    }
  }
  pt=minp;
  vec=minvec;
  return minl;
}


main(int argc, char **argv){

  if (argc!=4){
    std::cerr << "usage: " << argv[0] << " <conf_file> <in.fig> <out.fig>\n";
    exit(0);
  }
  string conf_file = argv[1];
  string infile    = argv[2];
  string outfile   = argv[3];

  zn::zn_conv zconverter(conf_file);

  fig_world W = read(infile.c_str());
  fig_world NW;

  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){

    if ((i->type == 6) || (i->type == -6)) continue;

    if ((i->comment.size()>1) && (i->comment[1] == "[skip]")) continue;

    if ((i->depth >=30) && (i->depth<50)) {NW.push_back(*i); continue;}

    if ((i->depth <30) || (i->depth>=400)) continue;

    if (i->size() == 0) continue;

    zconverter.fig_update(*i);    

    // преобразуем некоторые объекты в x-spline
    zn::zn_key k = zn::get_key(*i);
    if (
         (k.type == 0x100001) || // автомагистраль
         (k.type == 0x100002) || // шоссе
         (k.type == 0x100003) || // верхний край обрыва
         (k.type == 0x100004) || // грейдер
         (k.type == 0x100006) || // черная дорога
         (k.type == 0x100007) || // непроезжий грейдер
         (k.type == 0x10000C) || // хребет
         (k.type == 0x100015) || // река-1
         (k.type == 0x100018) || // река-2
         (k.type == 0x10001E) || // нижний край обрыва
         (k.type == 0x10001F) || // река-3
         (k.type == 0x100020) || // горизонталь пунктирная
         (k.type == 0x100021) || // горизонталь обычная
         (k.type == 0x100022) || // горизонталь жирная
         (k.type == 0x100025) || // овраг
         (k.type == 0x100026) || // пересыхающий ручей
         (k.type == 0x100027) || // ж/д
         (k.type == 0x10002A) || // тропа
         (k.type == 0x10002B) || // сухая канава
         (k.type == 0x100032) || // плохой путь
         (k.type == 0x100033) || // 
         (k.type == 0x100034) || // 
         (k.type == 0x100035) || // отличнй путь
         (k.type == 0x200014) || // редколесье
         (k.type == 0x200015) || // остров леса
         (k.type == 0x200029) || // озеро
         (k.type == 0x20003B) || // большое озеро
         (k.type == 0x20004D) || // ледник
         (k.type == 0x200052) || // поле
         (k.type == 0x200053)    // остров
       ) i->any2xspl(1);


    if (k.type == 0x1000){ // отметка уреза воды
      *i = make_object(*i, "1 3 0 1 22046463 7 57 -1 20 2.000 1 0.000 * * 23 23 * * * *");
      i->center_x = (*i)[0].x;
      i->center_y = (*i)[0].y;
      NW.push_back(*i); continue;
    }
    if (k.type == 0x2800){ // подпись урочища
      i->pen_color=0;  
      NW.push_back(*i); continue;
    }

    if (k.type == 0x5905){ // платформа
      Point<double> t, p((*i)[0]);
      nearest_line(W, 0x100027, t, p);

      Point<double> n(-t.y,t.x);
      fig_object o = make_object(*i, "2 3 0 1 0 7 * * 20 * 0 0 0 0 0 *");
      o.clear();
      t *= 80, n *= 35; // длина и ширина
      o.push_back(p+t+n);
      o.push_back(p+t-n);
      o.push_back(p-t-n);
      o.push_back(p-t+n);
      NW.push_back(o); continue;
    }

/*
    // порог
    if (k.type == 0x650E){
      
      Point<double> t, p((*i)[0]);

      double d1 = nearest_line(W, 0x100015, t1, p1);
      double d2 = nearest_line(W, 0x100018, t2, p2);
      double d3 = nearest_line(W, 0x10001F, t3, p3);
      double d4 = nearest_line(W, 0x100026, t4, p4);

      if ( W.nearest_pt(v2, p2, "2 * 0 * 22046463 * 86 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 22046463 * 86 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig_object o = make_object(*i, "2 1 0 2 1 0 * * 0 * 0 1 0 0 0 *");
      o.clear();
      double w = 30; // длина штриха
      o.push_back(Point<int>(int(p1.x + v2.x*w),int(p1.y + v2.y*w)));
      o.push_back(Point<int>(int(p1.x - v2.x*w),int(p1.y - v2.y*w)));
      *i=o;
      continue;
    }
*/

    
    if ((k.type == 0x100001)|| // автомагистраль
        (k.type == 0x100002)|| // шоссе 
        (k.type == 0x100004)){ // грейдер
      i->pen_color=0; NW.push_back(*i);
      fig_object o = *i;
      o.pen_color = o.fill_color; o.depth--; o.thickness-=2; NW.push_back(o);
      if (o.thickness>2) {o.pen_color = 0; o.depth--; o.thickness=1;  NW.push_back(o);}
      continue;
    }
    if (k.type == 0x100007){ // непроезжий грейдер
      i->pen_color=0; i->cap_style=0; i->line_style=0; NW.push_back(*i);
      fig_object o = *i; o.line_style=2; o.style_val=6; o.depth--; o.pen_color=7; NW.push_back(o);
      o.line_style=0; o.thickness-=2; NW.push_back(o);
      continue;
    }
    if (k.type == 0x10000A){ // непроезжая грунтовка
      i->pen_color=0; i->type=2; i->sub_type=1;
      LineDist<int> ld(*i);
      while (!ld.is_end()){
        fig_object o = *i;
        o.set_points(ld.get_points(80));
        ld.move_frw(20);
        NW.push_back(o);
      }
      continue;
    }
    if (k.type == 0x10001F){ // река-3
      NW.push_back(*i);
      fig_object o = *i; o.pen_color = o.fill_color; o.depth--; o.thickness-=2; 
      NW.push_back(o);  continue;
    }

    if ((k.type == 0x100008) || (k.type == 0x100009) || (k.type == 0x10000B)){ // мост
        i->resize(2);
        Point<double> p1=(*i)[0], p2=(*i)[1];
        double ll = pdist(p1,p2);
        Point<double> vt = (p1-p2)/ll, vn(-vt.y, vt.x);

        double w = (i->thickness+2)*12/2.0; // ширина моста
        double l = 20.0;                    // длина "стрелок"

        fig_object o = make_object("2 1 0 0 0 7 77 * 20 * 0 0 0 0 0 *");
        o.push_back(p1+vn*w);
        o.push_back(p2+vn*w);
        o.push_back(p2-vn*w);
        o.push_back(p1-vn*w);
        NW.push_back(o); o.clear();

        o = make_object("2 1 0 1 0 7 76 * -1 * 0 0 0 0 0 *");
        o.push_back(p1+vn*w+(vn+vt)*l);
        o.push_back(p1+vn*w);
        o.push_back(p2+vn*w);
        o.push_back(p2+vn*w+(vn-vt)*l);
        NW.push_back(o); o.clear();

        o.push_back(p1-vn*w-(vn-vt)*l);
        o.push_back(p1-vn*w);
        o.push_back(p2-vn*w);
        o.push_back(p2-vn*w-(vn+vt)*l);
        NW.push_back(o); continue;
    }

    if (k.type == 0x10001B){ // пешеходный тоннель

        i->resize(2);
        Point<double> p1=(*i)[0], p2=(*i)[1];
        double ll = pdist(p1,p2);
        Point<double> vt = (p1-p2)/ll, vn(-vt.y, vt.x);

        double l = 30.0;                    // длина "стрелок"

        fig_object o = make_object("2 1 0 1 0 7 81 * -1 * 0 0 0 0 0 *");
        o.push_back(p1);
        o.push_back(p2);
        NW.push_back(o); o.clear();
        
        o.push_back(p1+(vt+vn)*l);
        o.push_back(p1);
        o.push_back(p1+(vt-vn)*l);
        NW.push_back(o); o.clear();

        o.clear();
        o.push_back(p2-(vt+vn)*l);
        o.push_back(p2);
        o.push_back(p2-(vt-vn)*l);
        NW.push_back(o); continue;
    }

    if ((k.type == 0x100029)||(k.type == 0x10001A)){ // ЛЭП
      NW.push_back(*i);
      double step = 400;
      fig_object o = make_object(*i,
        "2 1 0 2 25725064 7 82 -1 -1 0.000 0 0 -1 1 1 * 0 0 2.00 90.00 90.00 0 0 2.00 90.00 90.00");
      o.farrow_width = (i->thickness<3)? 60:90;
      o.barrow_width = (i->thickness<3)? 60:90;
      o.farrow_height = (i->thickness<3)? 60:90;
      o.barrow_height = (i->thickness<3)? 60:90;
      double w = (i->thickness<3)? 40:60; // ширина черточек
      
      LineDist<int> ld(*i); ld.move_frw(step/4);
      int n=1;
      while (ld.dist() < ld.length()-step/4){
        Point<double> v, p=ld.pt();
        if (n%2 == 0){ o.forward_arrow = 0; o.backward_arrow = 0; v=ld.norm();}
        if (n%4 == 1){ o.forward_arrow = 1; o.backward_arrow = 0; v=ld.tang();}
        if (n%4 == 3){ o.forward_arrow = 0; o.backward_arrow = 1; v=ld.tang();}
        o.clear();
        o.push_back(p+v*w);
        o.push_back(p-v*w);
        NW.push_back(o);
        n++;
        ld.move_frw(step);
      }
      continue;
    }

    if (k.type == 0x100028){ // газопровод
      i->line_style=0;
      NW.push_back(*i);
      double step = 600;
      fig_object o = make_object(*i, "1 3 0 1 25725064 7 82 -1 20 0.000 1 0.0000 * * 40 40 * * * *");
      
      LineDist<int> ld(*i); ld.move_frw(step/4);
      int n=0;
      while (ld.dist() < ld.length()-step/4){
        Point<double> p=ld.pt();
        o.clear();
        o.center_x = p.x;
        o.center_y = p.y;
        NW.push_back(o);
        n++;
        ld.move_frw(step);
      }
      continue;
    }




    if (k.type == 0x20003B){ // большой водоем
      i->area_fill = 20; NW.push_back(*i); continue;
    }
    if (k.type == 0x200001){ // город
      i->area_fill = 20; NW.push_back(*i); continue;
    }
    if (k.type == 0x20001A){ // кладбище
      i->area_fill=10;
      int w1=23, w2=45; // размер крестика

      // ищем середину объекта
      Point<int> pmin = (*i)[0];
      Point<int> pmax = (*i)[0];
      for (int j = 0; j<i->size(); j++){
        if (pmin.x > (*i)[j].x) pmin.x = (*i)[j].x;
        if (pmin.y > (*i)[j].y) pmin.y = (*i)[j].y;
        if (pmax.x < (*i)[j].x) pmax.x = (*i)[j].x;
        if (pmax.y < (*i)[j].y) pmax.y = (*i)[j].y;
      }
      Point<int> p=(pmin+pmax)/2;
      p.y-=(w2-w1)/2;

      fig_object o = make_object("2 1 0 1 0 * 57 * * * * 0 * * * *");
      o.push_back(p+Point<int>(-w1,0));
      o.push_back(p+Point<int>(+w1,0));
      NW.push_back(o);
      o.clear();
      o.push_back(p+Point<int>(0,-w1));
      o.push_back(p+Point<int>(0,+w2));
      NW.push_back(o);
      NW.push_back(*i); continue;
    }
 
   if ((k.type == 0x20004C) || (k.type == 0x200051)){ // болота
      int step = 40; // расстояние между штрихами

      // ищем габариты объекта
      Point<int> pmin = (*i)[0];
      Point<int> pmax = (*i)[0];
      Line<double> c;
      for (int j = 0; j<i->size(); j++){
        c.push_back((*i)[j]);
        if (pmin.x > (*i)[j].x) pmin.x = (*i)[j].x;
        if (pmin.y > (*i)[j].y) pmin.y = (*i)[j].y;
        if (pmax.x < (*i)[j].x) pmax.x = (*i)[j].x;
        if (pmax.y < (*i)[j].y) pmax.y = (*i)[j].y;
      }
      list<Line<double> > ls;
      int n=0;
      for (int y = pmin.y; y< pmax.y; y+=step){
        Line<double> l; 
        if (n%2==0){
          l.push_back(Point<int>(pmin.x, y));
          l.push_back(Point<int>(pmax.x, y));
        } else {
          l.push_back(Point<int>(pmax.x, y));
          l.push_back(Point<int>(pmin.x, y));
        }
        ls.push_back(l);
        n++;
      }
      crop_lines(ls, c);
      fig_object o = make_object("2 1 0 1 22046463 7 87 -1 -1 0.000 0 1 0 0 0 0");
      if (k.type == 0x200051) {o.line_style=1; o.style_val=5;}
      for (list<Line<double> >::iterator l = ls.begin(); l!=ls.end(); l++){
        o.set_points(*l);
        NW.push_back(o);
      }
      continue;
   }

    // прочие объекты - без изменений
    list<fig_object> l1 = zconverter.make_pic(*i);
    NW.insert(NW.end(), l1.begin(), l1.end());
  }
//  text_bbxs(W);
  ofstream out(outfile.c_str());
  write(out, NW);
}
