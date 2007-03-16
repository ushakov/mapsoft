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

  for (fig::fig_world::iterator i=W.begin(); i!=W.end(); i++){
    // автомагистраль
    if (fig::test_object(*i, "2 * 0 7 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 7 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.depth = 78; o.thickness = 1; W.push_back(o);
      o.pen_color = 27; o.depth = 79; o.thickness = 5; W.push_back(o);
    }
    // шоссе
    if (fig::test_object(*i, "2 * 0 4 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 4 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 27; o.depth = 79; o.thickness = 2; W.push_back(o);
    }
    // грейдер
    if (fig::test_object(*i, "2 * 0 3 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; W.push_back(o);
    }
    // непроезжий грейдер
    if (fig::test_object(*i, "2 * 1 3 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 1 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; o.line_style=0; W.push_back(o);
    }
    // непроезжая грунтовка
    if (fig::test_object(*i, "2 * 0 1 34 * 80 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 1 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig::fig_object o = *i;
      o.pen_color  = 7; o.depth = 79; 
      o.cap_style  = 2; o.line_style = 2;
      o.style_val  = 8.0; W.push_back(o);
    }
    // река-5
    if (fig::test_object(*i, "2 * 0 5 33 * 85 * * * * * * * * *") ||
        fig::test_object(*i, "3 * 0 5 33 * 85 * * * * * * *")) {
      fig::fig_object o = *i;
      o.pen_color = 3; o.depth = 84; o.thickness = 3; W.push_back(o);
    }
    // платформа
    if (fig::test_object(*i, "2 * * * 4 * 57 * * * * * * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 3 0 * 80 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 3 0 * 80 * * * * * * *")){
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
    }
    // порог
    if (fig::test_object(*i, "2 * * * 8 * 57 * * * * * * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 85 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 85 * * * * * * *")){
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
    }
    // водопад
    if (fig::test_object(*i, "2 * * * 17 * 57 * * * * * * * * 1")){
      Point<double> p1(i->x[0], i->y[0]), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 85 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 85 * * * * * * *")){
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
    }
    // отметка уреза воды
    if (fig::test_object(*i, "2 * * * 1 * 57 * * * * 1 * 0 0 1")){
      *i = fig::make_object(*i, "1 3 0 1 33 7 57 -1 20 2.000 1 0.000 * * 23 23 * * * *");
      i->center_x = i->start_x = i->end_x = i->x[0];
      i->center_y = i->start_y = i->end_y = i->y[0];
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
    if ((fig::test_object(*i, "2 * * * * * 50 * * * * * * * * *"))||
        (fig::test_object(*i, "3 * * * * * 50 * * * * * * * "))){
      string::size_type size=i->comment.size(), idx=i->comment.find("\n",0);
      if ((size<2)||(idx==string::npos)||min(i->x.size(),i->y.size())<2) continue;
      string text = i->comment.substr(2,idx-2);
      double l=0;
      vector<double> ls;
      vector<Point<double> > vs;
      for (int j=1; j<min(i->x.size(),i->y.size()); j++){
        Point<double> p1 (i->x[j-1], i->y[j-1]);
        Point<double> p2 (i->x[j], i->y[j]);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        l+=dl; ls.push_back(l);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }
      double dl=l/text.size(); l=0;
      Point<double> p(i->x[0], i->y[0]);
      p.x-=vs[0].y*7.5*i->thickness;
      p.y+=vs[0].x*7.5*i->thickness;
      double a = atan2(vs[0].y, vs[0].x);
      int seg=0;

      for (int j=0; j<text.size(); j++){
        fig::fig_object o = fig::make_object(*i, "4 0 * * * 18 * * 4");
        o.font_size=i->thickness;
        o.angle=-a;
        o.x.clear(); o.y.clear();
        o.x.push_back(int(p.x));
        o.y.push_back(int(p.y));
        o.text=text[j];
        W.push_back(o);
        l+=dl;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        a = atan2(vs[seg].y,vs[seg].x);
        p.x = i->x[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = i->y[seg] + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
        p.x-=vs[seg].y*7.5*i->thickness;
        p.y+=vs[seg].x*7.5*i->thickness;
      }
    }

  }
  fig::write(out, W);
}
