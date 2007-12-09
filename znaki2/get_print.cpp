#include <iostream>
#include <fstream>
#include "../geo_io/fig.h"
#include <cmath>
#include "zn.h"
#include "line_dist.h"

using namespace std;
using namespace fig;


// Найти ближайшую к точке pt линию из списка.
// В pt запихать эту ближайшую точку, в vec - едиичный вектор направления линии,
// вернуть расстояние. 
// Поиск происходит на расстоянии не более maxdist (xfig units)
double nearest_line(const list<Line<int> > & lines, Point<double> & vec, Point<double> & pt, double maxdist=100){

  Point<double> minp(pt),minvec(1,0);
  double minl=maxdist;

  for (list<Line<int> >::const_iterator i  = lines.begin(); i != lines.end(); i++){
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


  // для убыстрения делаем два прогона - в первом распихиваем 
  // по  отдельным спискам все линейные объекты, к которым мы хотим привязывать точки:
  // ж/д, реки и хребты. Во втором уже делаем собственно преобразование объектов...

  list<Line<int> > list_zd, list_r, list_h;

  for (int pass =0 ; pass<2; pass++)
  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){
    if ((i->type == 6) || (i->type == -6)) continue;
    if ((i->comment.size()>1) && (i->comment[1] == "[skip]")) continue;
    if ((i->depth >=30) && (i->depth<50)) {NW.push_back(*i); continue;}
    if ((i->depth <30) || (i->depth>=400)) continue;
    if (i->size() == 0) continue;

    zn::zn_key k = zn::get_key(*i);

    if (pass ==0){
      if (k.type == 0x100027) list_zd.push_back(*i);
      if ((k.type == 0x100015)||
          (k.type == 0x100018)||
          (k.type == 0x10001F)|| 
          (k.type == 0x100026)) list_r.push_back(*i);
      if (k.type == 0x10000C)   list_h.push_back(*i);
      continue;
    }


    zconverter.fig_update(*i);    
    // преобразуем некоторые объекты в x-spline
    if (
         (k.type == 0x100001) || // автомагистраль
         (k.type == 0x100002) || // шоссе
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

    // отметка уреза воды
    if (k.type == 0x1000){ 
      *i = make_object(*i, "1 3 0 1 22046463 7 57 -1 20 2.000 1 0.000 * * 23 23 * * * *");
      i->center_x = (*i)[0].x;
      i->center_y = (*i)[0].y;
      NW.push_back(*i); continue;
    }
    // подпись урочища
    if (k.type == 0x2800){
      i->pen_color=0;
      i->depth=40; //! чтоб под текстом линии стирались!
      NW.push_back(*i); continue;
    }
    // платформа
    if (k.type == 0x5905){
      Point<double> t, p((*i)[0]);
      nearest_line(list_zd, t, p);

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
    // все перевалы разом!
    if ((k.type == 0x6406)||
        (k.type == 0x6621)||
        (k.type == 0x6622)||
        (k.type == 0x6623)||
        (k.type == 0x6624)||
        (k.type == 0x6625)||
        (k.type == 0x6626)){
      Point<double> t, p((*i)[0]);
      nearest_line(list_h, t, p);
      list<fig_object> l1 = zconverter.make_pic(*i);
      fig::fig_rotate(l1, atan2(t.y, t.x), p);
      NW.insert(NW.end(), l1.begin(), l1.end());
      continue;
    }
    // порог и водопад
    if ((k.type == 0x650E) || (k.type == 0x6508)){
      
      Point<double> t1, p1((*i)[0]);
      nearest_line(list_r, t1, p1);

      Point<double> n(-t1.y,t1.x);
      fig_object o = make_object("2 1 0 2 1 0 57 -1 0 0 0 1 0 0 0 *");
      double w = 30; // длина штриха
      if (k.type == 0x6508) o.thickness=3;
      o.push_back(p1+n*w);
      o.push_back(p1-n*w);
      NW.push_back(o);
      continue;
    }

    // автомагистраль, шоссе, грейдер
    if ((k.type == 0x100001)||
        (k.type == 0x100002)||
        (k.type == 0x100004)){
      i->pen_color=0; NW.push_back(*i); 
      fig_object o = *i;
      o.pen_color = o.fill_color; o.depth--; o.thickness-=2; NW.push_back(o);
      if (o.thickness>2) {o.pen_color = 0; o.depth--; o.thickness=1;  NW.push_back(o);}
      continue;
    }
    // непроезжий грейдер
    if (k.type == 0x100007){
      i->pen_color=0; i->cap_style=0; i->line_style=0; NW.push_back(*i);
      fig_object o = *i; o.line_style=2; o.style_val=6; o.depth--; o.pen_color=7; NW.push_back(o);
      o.line_style=0; o.thickness-=2; NW.push_back(o);
      continue;
    }
    // непроезжая грунтовка
    if (k.type == 0x10000A){
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
    // река-3
    if (k.type == 0x10001F){ 
      NW.push_back(*i);
      fig_object o = *i; o.pen_color = o.fill_color; o.depth--; o.thickness-=2; 
      NW.push_back(o);  continue;
    }

    // мост
    if ((k.type == 0x100008) || (k.type == 0x100009) || (k.type == 0x10000B)){ 
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

    // пешеходный тоннель
    if (k.type == 0x10001B){ 

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

    // ЛЭП
    if ((k.type == 0x100029)||(k.type == 0x10001A)){ 
      NW.push_back(*i);
      double step = 400;
      fig_object o = make_object(*i,
        "2 1 0 2 25725064 7 82 -1 -1 0.000 0 0 -1 1 1 * 0 0 2.00 90.00 90.00 0 0 2.00 90.00 90.00");
      o.farrow_width = (i->thickness<3)? 60:90;
      o.barrow_width = (i->thickness<3)? 60:90;
      o.farrow_height = (i->thickness<3)? 60:90;
      o.barrow_height = (i->thickness<3)? 60:90;
      double w = (i->thickness<3)? 40:60; // ширина черточек
      
      LineDist<int> ld(*i); 
      if (ld.length()<=step) step = ld.length();
      else step = ld.length()/floor(ld.length()/step);
      ld.move_frw(step/2);
      int n=1;
      while (ld.dist() < ld.length()){
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

    // газопровод
    if (k.type == 0x100028){
      i->line_style=0;
      NW.push_back(*i);
      double step = 600;
      fig_object o = make_object(*i, "1 3 0 1 25725064 7 82 -1 20 0.000 1 0.0000 * * 40 40 * * * *");
      
      LineDist<int> ld(*i); 
      if (ld.length()<=step) step = ld.length();
      else step = ld.length()/floor(ld.length()/step);
      ld.move_frw(step/2);
      while (ld.dist() < ld.length()){
        Point<double> p=ld.pt();
        o.clear();
        o.center_x = p.x;
        o.center_y = p.y;
        NW.push_back(o);
        ld.move_frw(step);
      }
      continue;
    }

    // кривая надпись
    if (k.type == 0x100000){ 
      if ((i->size()<2)||(i->comment.size()<1)||(i->comment[0].size()<1)) continue;

      LineDist<int> ld(*i); 
      double shift = 7.5*i->thickness;

      fig_object o = make_object(*i, "4 1 * * * 3 * * 4");
      o.font_size=i->thickness;

      double step=ld.length()/i->comment[0].size();

      ld.move_frw(step/2);
      for (int n=0; n<i->comment[0].size(); n++){
        Point<double> p=ld.pt(), t=ld.tang();
        o.text=i->comment[0][n];
        o.angle = -atan2(t.y, t.x);
        o.clear(); o.push_back(p);
        NW.push_back(o);
        ld.move_frw(step);
      }
      continue;
    }

    // забор
    if (k.type == 0x100019){ 
      double step = 130;
      double w    = 30;
      int k=1;
      if (i->backward_arrow==1) k=-1;

      i->pen_color=0;
      i->forward_arrow=0;
      i->backward_arrow=0;
      i->type=2; i->sub_type=1;
      NW.push_back(*i);
      fig_object o = *i; o.clear();

      LineDist<int> ld(*i);
      if (ld.length()<=step) step = ld.length();
      else step = ld.length()/floor(ld.length()/step);
      ld.move_frw((step-w)/2);
      int n=0;
      while (ld.dist() < ld.length()){
        Point<double> p=ld.pt(), vt=ld.tang(), vn=ld.norm();
        o.clear();
        o.push_back(p);
        o.push_back(p+k*(vn-vt)*w);
        NW.push_back(o);
        ld.move_frw((n%2==0) ? w:(step-w));
        n++;
      }
      continue;
    }

    // верхний край обрыва
    if (k.type == 0x100003){ 
      double step = 40;
      double w    = 20;
      int k=1;
      if (i->backward_arrow==1) k=-1;

      i->forward_arrow=0;
      i->backward_arrow=0;
      i->cap_style=1;
      i->type=2; i->sub_type=1;
      NW.push_back(*i);
      fig_object o = *i; o.clear();

      LineDist<int> ld(*i);
      if (ld.length()<=step) step = ld.length();
      else step = ld.length()/floor(ld.length()/step);
      ld.move_frw(step/2);

      while (ld.dist() < ld.length()){
        Point<double> p=ld.pt(), vn=ld.norm();
        o.clear();
        o.push_back(p);
        o.push_back(p+k*vn*w);
        NW.push_back(o);
        ld.move_frw(step);
      }
      continue;
    }
    



    // большой водоем
    if (k.type == 0x20003B){ 
      i->area_fill = 20; NW.push_back(*i); continue;
    }
    // город
    if (k.type == 0x200001){ 
      i->area_fill = 20; NW.push_back(*i); continue;
    }
    // кладбище
    if (k.type == 0x20001A){ 
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
 
   // болота
   if ((k.type == 0x20004C) || (k.type == 0x200051)){ 
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
