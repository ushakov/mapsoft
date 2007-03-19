#include <iostream>
#include <fstream>
#include "fig.h"
#include <cmath>

using namespace std;

main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " in.fig out.fig\n";
    exit(0);
  }
  string infile  = argv[1];
  string outfile = argv[2];

  ofstream out(outfile.c_str());

  fig::fig_world W = fig::read(argv[1]);

  // удалим объекты с глубиной >=200 и <10;
  fig::fig_world::iterator i=W.begin();
  while (i!=W.end()){
    if ((i->depth>=200)||(i->depth<10)) i=W.erase(i);
    else i++;
  } 

  // преобразуем некоторые объекты в x-spline с параметром 0.4
  for (fig::fig_world::iterator i=W.begin(); i!=W.end(); i++){
    int fs=min(i->x.size(),i->y.size());
    if ((fs>2) && (
      fig::test_object(*i, "2 * * *  * * 84 * * * * * * * * *") || // водоемы
      fig::test_object(*i, "2 * * *  * * 85 * * * * * * * * *") ||
      fig::test_object(*i, "2 * * *  * * 86 * * * * * * * * *") ||
      fig::test_object(*i, "2 * * *  * * 96 * * * * * * * * *") || // леса/поля
      fig::test_object(*i, "2 * * *  * * 97 * * * * * * * * *") ||
      fig::test_object(*i, "2 * * *  * * 98 * * * * * * * * *") ||
      fig::test_object(*i, "2 * * *  * * 99 * * * * * * * * *") ||
      fig::test_object(*i, "2 * * * 34 * 80 * * * * * * * * *") || // серые дороги
      fig::test_object(*i, "2 * 0 *  0 * 80 * * * * * * * * *") || // черные непунктирные дороги
      fig::test_object(*i, "2 * 3 *  0 * 80 * * * * * * * * *") || // границы
      fig::test_object(*i, "2 * 2 *  0 * 80 * * * * * * * * *") || // тропы
      fig::test_object(*i, "2 * * * 18 * 79 * * * * * * * * *") || // обрывы
      fig::test_object(*i, "2 * * *  * * 89 * * * * * * * * *") || // овраги, хребты
      fig::test_object(*i, "2 * * *  * * 90 * * * * * * * * *") || // горизонтали

      fig::test_object(*i, "3 * * *  * * 84 * * * * * * *") || // водоемы
      fig::test_object(*i, "3 * * *  * * 85 * * * * * * *") ||
      fig::test_object(*i, "3 * * *  * * 86 * * * * * * *") ||
      fig::test_object(*i, "3 * * *  * * 96 * * * * * * *") || // леса/поля
      fig::test_object(*i, "3 * * *  * * 97 * * * * * * *") ||
      fig::test_object(*i, "3 * * *  * * 98 * * * * * * *") ||
      fig::test_object(*i, "3 * * *  * * 99 * * * * * * *") ||
      fig::test_object(*i, "3 * * * 34 * 80 * * * * * * *") || // серые дороги
      fig::test_object(*i, "3 * 0 *  0 * 80 * * * * * * *") || // черные непунктирные дороги
      fig::test_object(*i, "3 * 3 *  0 * 80 * * * * * * *") || // границы
      fig::test_object(*i, "3 * 2 *  0 * 80 * * * * * * *") || // тропы
      fig::test_object(*i, "3 * * * 18 * 79 * * * * * * *") || // обрывы
      fig::test_object(*i, "3 * * *  * * 89 * * * * * * *") || // овраги, хребты
      fig::test_object(*i, "3 * * *  * * 90 * * * * * * *")    // горизонтали
    )){
      if (i->type==2){ 
        if (i->sub_type<=1) i->sub_type=4; else i->sub_type=5;
      }
      if (i->type==3){ 
        i->sub_type = (i->sub_type%2)+4;
      }
      if ((i->x[0]==i->x[fs-1]) && (i->y[0]==i->y[fs-1])){
        i->x.erase(i->x.begin()+fs-1);
        i->y.erase(i->y.begin()+fs-1);
        fs=min(i->x.size(),i->y.size());
      }

      i->type=3;
      i->f.clear();
  
      for (int j=0; j< fs; j++) i->f.push_back(0.3);
      if (i->sub_type==4){
        i->f[0]=0;
        i->f[fs-1]=0;
      }
    }
  }



  for (fig::fig_world::iterator i=W.begin(); i!=W.end(); i++){
    int fs=min(i->x.size(),i->y.size());
    // автомагистраль
    if (fig::test_object(*i, "2 * 0 7 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 7 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.depth = 78; o.thickness = 1; W.push_back(o);
      o.pen_color = 27; o.depth = 79; o.thickness = 5; W.push_back(o);
      continue;
    }
    // шоссе
    if (fig::test_object(*i, "2 * 0 4 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 4 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 27; o.depth = 79; o.thickness = 2; W.push_back(o);
      continue;
    }
    // грейдер
    if (fig::test_object(*i, "2 * 0 3 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; W.push_back(o);
      continue;
    }
    // непроезжий грейдер
    if (fig::test_object(*i, "2 * 1 3 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 1 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; o.line_style=0; W.push_back(o);
      continue;
    }
    // непроезжая грунтовка
    if (fig::test_object(*i, "2 * 0 1 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 1 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color  = 7; o.depth = 79; 
      o.cap_style  = 2; o.line_style = 2;
      o.style_val  = 8.0; W.push_back(o);
      continue;
    }
    // река-5
    if (fig::test_object(*i, "2 * 0 5 33 * 86 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 5 33 * 86 * * * * * * *")) {
      fig::fig_object o = *i;
      o.pen_color = 3; o.depth = 84; o.thickness = 3; W.push_back(o);
      continue;
    }
    // платформа
    if (fig::test_object(*i, "2 * * * 4 * 57 * * * * 0 * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 4 0 * 80 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 4 0 * 80 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig::fig_object o = fig::make_object(*i, "2 1 0 1 0 7 * * 20 * 0 0 0 0 0 *");
      o.x.clear(); o.y.clear();
      double l = 80, w=40; // ширина и длина станции
      o.x.push_back(int(p1.x + v1.x*l + v2.x*w));
      o.x.push_back(int(p1.x + v1.x*l - v2.x*w));
      o.x.push_back(int(p1.x - v1.x*l - v2.x*w));
      o.x.push_back(int(p1.x - v1.x*l + v2.x*w));
      o.x.push_back(int(p1.x + v1.x*l + v2.x*w));
      o.y.push_back(int(p1.y + v1.y*l + v2.y*w));
      o.y.push_back(int(p1.y + v1.y*l - v2.y*w));
      o.y.push_back(int(p1.y - v1.y*l - v2.y*w));
      o.y.push_back(int(p1.y - v1.y*l + v2.y*w));
      o.y.push_back(int(p1.y + v1.y*l + v2.y*w));
      *i=o; // сюда бы еще поправки на сплайны...
      continue;
    }
    // порог
    if (fig::test_object(*i, "2 * * * 8 * 57 * * * * 0 * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 86 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 86 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig::fig_object o = fig::make_object(*i, "2 1 0 2 1 0 * * 0 * 0 1 0 0 0 *");
      o.x.clear(); o.y.clear();
      double w = 30; // длина штриха
      o.x.push_back(int(p1.x + v2.x*w));
      o.x.push_back(int(p1.x - v2.x*w));
      o.y.push_back(int(p1.y + v2.y*w));
      o.y.push_back(int(p1.y - v2.y*w));
      *i=o; // сюда бы еще поправки на сплайны...
      continue;
    }
    // водопад
    if (fig::test_object(*i, "2 * * * 17 * 57 * * * * 0 * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 86 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 86 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig::fig_object o = fig::make_object(*i, "2 1 0 2 1 0 * * 0 * 0 1 0 0 0 *");
      double w = 30; // ширина крестика
      double l = 20; // длина крестика
      o.x.clear(); o.y.clear();
      o.x.push_back(int(p1.x + v2.x*w - v1.x*l));
      o.x.push_back(int(p1.x - v2.x*w + v1.x*l));
      o.y.push_back(int(p1.y + v2.y*w - v1.y*l));
      o.y.push_back(int(p1.y - v2.y*w + v1.y*l));
      *i=o;
      o.x.clear(); o.y.clear();
      o.x.push_back(int(p1.x + v2.x*w + v1.x*l));
      o.x.push_back(int(p1.x - v2.x*w - v1.x*l));
      o.y.push_back(int(p1.y + v2.y*w + v1.y*l));
      o.y.push_back(int(p1.y - v2.y*w - v1.y*l));
      W.push_back(o);
      continue;
    }
    // отметка уреза воды
    if (fig::test_object(*i, "2 * * * 1 * 57 * * * * 1 * 0 0 1")){
      *i = fig::make_object(*i, "1 3 0 1 33 7 57 -1 20 2.000 1 0.000 * * 23 23 * * * *");
      i->center_x = i->start_x = i->end_x = i->x[0];
      i->center_y = i->start_y = i->end_y = i->y[0];
      continue;
    }
    // автобусная остановка
    if (fig::test_object(*i, "2 * * * 4 * 57 * * * * 1 * * * 1")){
      int x = i->x[0];
      int y = i->y[0];
      *i = fig::make_object(*i, "4 1 4 55 -1 18 6 0.0000 4");
      i->y[0]+=35;
      i->text="A";
      continue;
    }

    // текст, не привязанный к объекту
    if (fig::test_object(*i, "4 1 12 55 -1 3 8 0.0000 4")){
      i->pen_color=0;
      continue;
    }

    // мост
    if (fig::test_object(*i, "2 * * * 7 * 77 * * * * * * 0 0 2")){

        Point<double> p1 (i->x[0], i->y[0]);
        Point<double> p2 (i->x[1], i->y[1]);
        double ll = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        Point<double> vt ((p1.x-p2.x)/ll, (p1.y-p2.y)/ll);
        Point<double> vn (-vt.y, vt.x);
        if (i->thickness>1) i->thickness+=2;
        double w = (i->thickness)*15/2.0; // ширина моста
        double l = 20.0;                    // длина "стрелок"

        *i=fig::make_object("2 1 0 0 0 7 77 * 20 * 0 0 0 0 0 *");
        i->x.push_back(int(p1.x + vn.x*w));
        i->y.push_back(int(p1.y + vn.y*w));
        i->x.push_back(int(p2.x + vn.x*w));
        i->y.push_back(int(p2.y + vn.y*w));
        i->x.push_back(int(p2.x - vn.x*w));
        i->y.push_back(int(p2.y - vn.y*w));
        i->x.push_back(int(p1.x - vn.x*w));
        i->y.push_back(int(p1.y - vn.y*w));

        fig::fig_object o = fig::make_object("2 1 0 1 0 7 76 * -1 * 0 0 0 0 0 *");
        o.x.clear(); o.y.clear();
        o.x.push_back(int(p1.x + vn.x*w + (vn.x+vt.x)*l));
        o.y.push_back(int(p1.y + vn.y*w + (vn.y+vt.y)*l));
        o.x.push_back(int(p1.x + vn.x*w));
        o.y.push_back(int(p1.y + vn.y*w));
        o.x.push_back(int(p2.x + vn.x*w));
        o.y.push_back(int(p2.y + vn.y*w));
        o.x.push_back(int(p2.x + vn.x*w + (vn.x-vt.x)*l));
        o.y.push_back(int(p2.y + vn.y*w + (vn.y-vt.y)*l));
        W.push_back(o);

        o.x.clear(); o.y.clear();
        o.x.push_back(int(p1.x - vn.x*w + (-vn.x+vt.x)*l));
        o.y.push_back(int(p1.y - vn.y*w + (-vn.y+vt.y)*l));
        o.x.push_back(int(p1.x - vn.x*w));
        o.y.push_back(int(p1.y - vn.y*w));
        o.x.push_back(int(p2.x - vn.x*w));
        o.y.push_back(int(p2.y - vn.y*w));
        o.x.push_back(int(p2.x - vn.x*w + (-vn.x-vt.x)*l));
        o.y.push_back(int(p2.y - vn.y*w + (-vn.y-vt.y)*l));
        W.push_back(o);
    }

    // туннель
    if (fig::test_object(*i, "2 * * * 3 * 77 * * * * * * 0 0 2")){

        Point<double> p1 (i->x[0], i->y[0]);
        Point<double> p2 (i->x[1], i->y[1]);
        double ll = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        Point<double> vt ((p1.x-p2.x)/ll, (p1.y-p2.y)/ll);
        Point<double> vn (-vt.y, vt.x);
        double l = 20.0;                    // длина "стрелок"

        i->pen_color=0;
        i->depth=81;

        fig::fig_object o = fig::make_object("2 1 0 1 0 7 81 * -1 * 0 0 0 0 0 *");
        o.x.clear(); o.y.clear();
        o.x.push_back(int(p1.x + (vn.x+vt.x)*l));
        o.y.push_back(int(p1.y + (vn.y+vt.y)*l));
        o.x.push_back(int(p1.x));
        o.y.push_back(int(p1.y));
        o.x.push_back(int(p1.x + (-vn.x+vt.x)*l));
        o.y.push_back(int(p1.y + (-vn.y+vt.y)*l));
        W.push_back(o);

        o.x.clear(); o.y.clear();
        o.x.push_back(int(p2.x + (vn.x-vt.x)*l));
        o.y.push_back(int(p2.y + (vn.y-vt.y)*l));
        o.x.push_back(int(p2.x));
        o.y.push_back(int(p2.y));
        o.x.push_back(int(p2.x + (-vn.x-vt.x)*l));
        o.y.push_back(int(p2.y + (-vn.y-vt.y)*l));
        W.push_back(o);
    }

    // кривой текст
    if ((fig::test_object(*i, "2 * * * * * 55 * * * * * * * * *"))||
        (fig::test_object(*i, "3 * * * * * 55 * * * * * * * "))){
      string::size_type size=i->comment.size(), idx=i->comment.find("\n",0);
      if ((size<2)||(idx==string::npos)||fs<2) continue;
      string text = i->comment.substr(2,idx-2);
      double l=0;
      vector<double> ls;
      vector<Point<double> > vs;
      vector<Point<double> > ps;
      ps.push_back(Point<double>(i->x[0], i->y[0]));
      for (int j=1; j<fs; j++){
        Point<double> p1 (i->x[j-1], i->y[j-1]);
        Point<double> p2 (i->x[j], i->y[j]);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        l+=dl; ls.push_back(l);
        ps.push_back(p2);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }
      double shift = 7.5*i->thickness;

      double dl=l/text.size(); l=dl/2;
      Point<double> p(i->x[0], i->y[0]);
      p.x-=vs[0].y*shift;
      p.y+=vs[0].x*shift;
      double a = atan2(vs[0].y, vs[0].x);
      int seg=0;

      fig::fig_object o = fig::make_object(*i, "4 1 * * * 3 * * 4");
      o.font_size=i->thickness;

      for (int j=0; j<text.size(); j++){
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        a = atan2(vs[seg].y,vs[seg].x);
        p.x = ps[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = ps[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
        p.x-=vs[seg].y*shift;
        p.y+=vs[seg].x*shift;
        o.angle=-a;
        o.x.clear(); o.y.clear();
        o.x.push_back(int(p.x));
        o.y.push_back(int(p.y));
        o.text=text[j];
        if (j==0) *i=o; else W.push_back(o);
        l+=dl;
      }
    }

    // ЛЭП
    if ((fig::test_object(*i, "2 * 0 * 35 * 83 * * * * * * * * *"))&&(fs>1)){
      double ll0=0;
      vector<double> ls;
      vector<Point<double> > vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 (i->x[j-1], i->y[j-1]);
        Point<double> p2 (i->x[j], i->y[j]);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }


      fig::fig_object o = fig::make_object(*i, "2 1 0 2 35 7 82 -1 -1 0.000 0 0 -1 1 1 * 0 0 2.00 90.00 90.00 0 0 2.00 90.00 90.00");
      o.farrow_width = (i->thickness<3)? 60:90;
      o.barrow_width = (i->thickness<3)? 60:90;
      o.farrow_height = (i->thickness<3)? 60:90;
      o.barrow_height = (i->thickness<3)? 60:90;
      double w = (i->thickness<3)? 40:60;

      double step = 400;
      double l=0;
      int n=0;
      Point<double> p(i->x[0], i->y[0]);
      int seg=0;

      while (l<ll0){
        Point<double> v;
        if (n%2 == 0){ o.forward_arrow = 0; o.backward_arrow = 0; v.x=-vs[seg].y; v.y=vs[seg].x;}
        if (n%4 == 1){ o.forward_arrow = 1; o.backward_arrow = 0; v.x=vs[seg].x; v.y=vs[seg].y;}
        if (n%4 == 3){ o.forward_arrow = 0; o.backward_arrow = 1; v.x=vs[seg].x; v.y=vs[seg].y;}
        o.x.clear(); o.y.clear();
        o.x.push_back(int(p.x+v.x*w));
        o.y.push_back(int(p.y+v.y*w));
        o.x.push_back(int(p.x-v.x*w));
        o.y.push_back(int(p.y-v.y*w));
        W.push_back(o);
        l+=step; n++;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        p.x = i->x[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = i->y[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
      }
    }

    // газопровод
    if ((fig::test_object(*i, "2 * 1 * 35 * 83 * * * * * * * * *"))&&(fs>1)){
      double ll0=0;
      vector<double> ls;
      vector<Point<double> > vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 (i->x[j-1], i->y[j-1]);
        Point<double> p2 (i->x[j], i->y[j]);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }

      i->line_style=0;
      fig::fig_object o = fig::make_object(*i, "1 3 0 1 35 7 82 -1 20 0.000 1 0.0000 * * 40 40 * * * *");

      double step = 600;
      double l=0;
      Point<double> p(i->x[0], i->y[0]);
      int seg=0;

      while (l<ll0){
        o.center_x = o.start_x = o.end_x = int(p.x);
        o.center_y = o.start_y = o.end_y = int(p.y);
        W.push_back(o);
        l+=step;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        p.x = i->x[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = i->y[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
      }
    }
    // обрыв
    if ((fig::test_object(*i, "3 * 2 * 18 * 79 * * * * * * *"))||
        (fig::test_object(*i, "2 * 2 * 18 * 79 * * * * * * * * *"))){
      i->thickness=0;
    }
    // обрыв
    if (((fig::test_object(*i, "3 * 0 * 18 * 79 * * * * * * *"))||
         (fig::test_object(*i, "2 * 0 * 18 * 79 * * * * * * * * *")))&&(fs>1)){
      // найдем ближайший низ обрыва, определим, взаимные направления...
      fig::fig_world::const_iterator no, nomin;
      double dmin=1e99, amin;
      int fs1;
      for (no = W.begin(); no != W.end(); no++){
        if (!test_object(*no, "3 * 2 * 18 * 79 * * * * * * *") && 
            !test_object(*no, "2 * 2 * 18 * 79 * * * * * * * * *")) continue;
        fs1=min(no->x.size(),no->y.size());
        double sum=0;
        int sx=0, sy=0,sxx=0, sxy=0;
        for (int j=0;j<fs;j++){
          double min=1e99; 
          int kmin=0;
          for (int k=0;k<fs1;k++){
            double l = sqrt((i->x[j]-no->x[k])*(i->x[j]-no->x[k]) + 
                            (i->y[j]-no->y[k])*(i->y[j]-no->y[k]));
            if (min>l) {min=l; kmin=k;}

          }
          sx+=j; sxx+=j*j; sxy+=j*kmin; sy+=kmin;
          sum+=min;
        }
        sum/=fs; // среднее минимальное расстояние от точки верха до точки низа
        if (sum<dmin) {dmin=sum; nomin=no; amin=double(sx*sy-sxy*fs)/double(sx*sx-sxx*fs);}
      }
      if (dmin>500) continue;

      double l0a=0, l0b=0;
      vector<double> lsa, lsb;
      vector<Point<double> > vsa, vsb, psa, psb;
      fs1=min(nomin->x.size(),nomin->y.size());

      for (int j=1; j<fs; j++){
        Point<double> p1 (i->x[j-1], i->y[j-1]);
        Point<double> p2 (i->x[j], i->y[j]);
        if (j==1) psa.push_back(p1);
        psa.push_back(p2);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        l0a+=dl; lsa.push_back(l0a);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vsa.push_back(v);
        }
        else  vsa.push_back(Point<double>(1,0));
      }

      for (int j=1; j<fs1; j++){
        Point<double> p1 (nomin->x[j-1], nomin->y[j-1]);
        Point<double> p2 (nomin->x[j], nomin->y[j]);
        if (amin<0){
          p1 = Point<double>(nomin->x[fs1-j], nomin->y[fs1-j]);
          p2 = Point<double>(nomin->x[fs1-j-1], nomin->y[fs1-j-1]);
        }
        if (j==1) psb.push_back(p1);
        psb.push_back(p2);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        l0b+=dl; lsb.push_back(l0b);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vsb.push_back(v);
        }
        else  vsb.push_back(Point<double>(1,0));
      }

      i->pen_color=0;
      fig::fig_object o = *i;
      o.type=2; o.sub_type=1;
      o.x.clear(); o.y.clear();

      double step = 50;

      double stepa = l0a/ (l0a+l0b)*2 * step;
      double stepb = l0b/ (l0a+l0b)*2 * step;

      double la=0, lb=0;
      Point<double> pa(psa[0]);
      Point<double> pb(psb[0]);
      int sega=0, segb=0;

      while ((la<=l0a)&&(lb<=l0b)){
        o.x.clear(); o.y.clear();
        o.x.push_back(int(pa.x));
        o.y.push_back(int(pa.y));
        o.x.push_back(int(pb.x));
        o.y.push_back(int(pb.y));
        W.push_back(o);
        
        la+=stepa;
        lb+=stepb;
        while ((lsa[sega]<=la)&&(sega<lsa.size())) sega++;
        while ((lsb[segb]<=lb)&&(segb<lsb.size())) segb++;
        pa.x = psa[sega].x + (la - (sega==0? 0: lsa[sega-1]))*vsa[sega].x;
        pa.y = psa[sega].y + (la - (sega==0? 0: lsa[sega-1]))*vsa[sega].y;
        pb.x = psb[segb].x + (lb - (segb==0? 0: lsb[segb-1]))*vsb[segb].x;
        pb.y = psb[segb].y + (lb - (segb==0? 0: lsb[segb-1]))*vsb[segb].y;
      }
    }
    // кладбище
    if (fig::test_object(*i, "2 * 0 1  0 32  92 *  5 * * * * * * *")){
      i->area_fill=10;
      int w1=23, w2=45;

      int minx=0x7FFFFFFF, maxx=-0x7FFFFFFF;
      int miny=0x7FFFFFFF, maxy=-0x7FFFFFFF;
      for (int j=0; j<fs; j++){ 
        if (i->x[j]>maxx) maxx=i->x[j];
        if (i->x[j]<minx) minx=i->x[j];
        if (i->y[j]>maxy) maxy=i->y[j];
        if (i->y[j]<miny) miny=i->y[j];
      }
      if ((minx>maxx)||(miny>maxy)) continue;

      int x=(maxx+minx)/2, y=(maxy+miny)/2-w1/2;

      fig::fig_object o = fig::make_object("2 1 0 1 0 * 57 * * * * 0 * * * *");
      o.x.push_back(x-w1); o.y.push_back(y);
      o.x.push_back(x+w1); o.y.push_back(y);
      W.push_back(o); o.x.clear(); o.y.clear();
      o.x.push_back(x); o.y.push_back(y-w1);
      o.x.push_back(x); o.y.push_back(y+w2);
      W.push_back(o);
    }


  }
  fig::write(out, W);
}
