#include <iostream>
#include <fstream>
#include "../geo_io/fig.h"
#include <cmath>

using namespace std;
using namespace fig;

main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " in.fig out.fig\n";
    exit(0);
  }
  string infile  = argv[1];
  string outfile = argv[2];

  ofstream out(outfile.c_str());

  fig_world W = read(argv[1]);

  // удалим объекты с глубиной >=200 и <10;
  fig_world::iterator i=W.begin();
  while (i!=W.end()){
    if ((i->depth>=200)||(i->depth<10)) i=W.erase(i);
    else i++;
  } 

  // преобразуем некоторые объекты в x-spline с параметром 0.4
  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){
    int fs=i->size();
    if ((fs>2) && (
      test_object(*i, "2 * * *  * * 84 * * * * * * * * *") || // водоемы
      test_object(*i, "2 * * *  * * 85 * * * * * * * * *") ||
      test_object(*i, "2 * * *  * * 86 * * * * * * * * *") ||
      test_object(*i, "2 * * * 34 * 80 * * * * * * * * *") || // серые дороги
      test_object(*i, "2 * 0 *  0 * 80 * * * * * * * * *") || // черные непунктирные дороги
      test_object(*i, "2 * 2 *  0 * 80 * * * * * * * * *") || // тропы
      test_object(*i, "2 * * * 18 * 79 * * * * * * * * *") || // обрывы
      test_object(*i, "2 * * *  * * 89 * * * * * * * * *") || // овраги, хребты
      test_object(*i, "2 * * *  * * 90 * * * * * * * * *") || // горизонтали

      test_object(*i, "3 * * *  * * 84 * * * * * * *") || // водоемы
      test_object(*i, "3 * * *  * * 85 * * * * * * *") ||
      test_object(*i, "3 * * *  * * 86 * * * * * * *") ||
      test_object(*i, "3 * * * 34 * 80 * * * * * * *") || // серые дороги
      test_object(*i, "3 * 0 *  0 * 80 * * * * * * *") || // черные непунктирные дороги
      test_object(*i, "3 * 2 *  0 * 80 * * * * * * *") || // тропы
      test_object(*i, "3 * * * 18 * 79 * * * * * * *") || // обрывы
      test_object(*i, "3 * * *  * * 89 * * * * * * *") || // овраги, хребты
      test_object(*i, "3 * * *  * * 90 * * * * * * *")    // горизонтали
    )){
      if (i->type==2){ 
        if (i->sub_type<=1) i->sub_type=4; else i->sub_type=5;
      }
      if (i->type==3){ 
        i->sub_type = (i->sub_type%2)+4;
      }
      if (((*i)[0].x==(*i)[fs-1].x) && ((*i)[0].y==(*i)[fs-1].y)){
        if (((i->type==2) && (i->sub_type==1))||
            ((i->type==3) && (i->sub_type==4))) i->sub_type=5;
//        i->erase(i->begin()+fs-1);
//        fs=i->size();
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



  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){
    int fs=i->size();
    // автомагистраль
    if (test_object(*i, "2 * 0 7 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 0 7 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig_object o = *i;
      o.depth = 78; o.thickness = 1; W.push_back(o);
      o.pen_color = 27; o.depth = 79; o.thickness = 5; W.push_back(o);
      continue;
    }
    // шоссе
    if (test_object(*i, "2 * 0 4 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 0 4 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig_object o = *i;
      o.pen_color = 27; o.depth = 79; o.thickness = 2; W.push_back(o);
      continue;
    }
    // грейдер
    if (test_object(*i, "2 * 0 3 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 0 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; W.push_back(o);
      continue;
    }
    // непроезжий грейдер
    if (test_object(*i, "2 * 1 3 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 1 3 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig_object o = *i;
      o.pen_color = 7; o.depth = 79; o.thickness = 1; o.line_style=0; W.push_back(o);
      continue;
    }
/*    // непроезжая грунтовка
    if (test_object(*i, "2 * 0 1 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 0 1 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      fig_object o = *i;
      o.pen_color  = 7; o.depth = 79; 
      o.cap_style  = 2; o.line_style = 2;
      o.style_val  = 8.0; W.push_back(o);
      continue;
    }*/

    // непроезжая грунтовка
    if (test_object(*i, "2 * 0 1 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 0 1 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      i->type=2;

      double ll0=0;
      vector<double> ls;
      Line<double> vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }

      i->line_style=0;
      fig_object o = make_object(*i, "2 1 0 1 0 7 80 -1 -1 0.000 0 0 -1 0 0 *");

      double s1 = 80;
      double s2 = 20;
      double l=0;
      int seg=0;
      Point<double> p;

      while (l<ll0){
        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s1;
        while ((ls[seg]<=l)&&(seg<ls.size())){ 
         seg++;
         o.push_back(Point<int>(int((*i)[seg].x),int((*i)[seg].y))); 
        }

        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        if (l<=ll0) o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s2;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++; }
        W.push_back(o);

      }
      i = W.erase(i); i--;
      continue;
    }

    // заросшая дорога 
    if (test_object(*i, "2 * 1 1 34 * 80 * * * * * * * * *") ||
        test_object(*i, "3 * 1 1 34 * 80 * * * * * * *")) {
      i->pen_color=0;
      i->type=2;

      double ll0=0;
      vector<double> ls;
      Line<double> vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }

      i->line_style=0;
      fig_object o = make_object(*i, "2 1 0 1 0 7 80 -1 -1 0.000 0 0 -1 0 0 *");

      double s1 = 30;
      double s2 = 15;
      double s3 = 40;
      double l=0;
      int seg=0;
      Point<double> p;

      while (l<ll0){
        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s1;
        while ((ls[seg]<=l)&&(seg<ls.size())){ 
         seg++;
         o.push_back(Point<int>(int((*i)[seg].x),int((*i)[seg].y))); 
        }

        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        if (l<=ll0) o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s2;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++; }
        W.push_back(o);

        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s1;
        while ((ls[seg]<=l)&&(seg<ls.size())){ 
         seg++;
         o.push_back(Point<int>(int((*i)[seg].x),int((*i)[seg].y))); 
        }

        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        if (l<=ll0) o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s3;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++; }
        W.push_back(o);


      }
      i = W.erase(i); i--;
      continue;
    }


    // река-5
    if (test_object(*i, "2 * 0 5 33 * 86 * * * * * * * * *") ||
        test_object(*i, "3 * 0 5 33 * 86 * * * * * * *")) {
      i->thickness = 3;
      fig_object o = *i;
      o.pen_color = 3; o.depth = 84; o.thickness = 1; W.push_back(o);
      continue;
    }

    // пунктирная река
    if (test_object(*i, "2 * 1 1 33 * 86 * * * * * * * * *") ||
        test_object(*i, "3 * 1 1 33 * 86 * * * * * * *")) {
      i->pen_color=33;
      i->type=2;

      double ll0=0;
      vector<double> ls;
      Line<double> vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }

      i->line_style=0;
      fig_object o = make_object(*i, "2 1 0 1 33 7 86 -1 -1 0.000 0 0 -1 0 0 *");

      double s1 = 80;
      double s2 = 20;
      double l=0;
      int seg=0;
      Point<double> p;

      while (l<ll0){
        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s1;
        while ((ls[seg]<=l)&&(seg<ls.size())){ 
         seg++;
         o.push_back(Point<int>(int((*i)[seg].x),int((*i)[seg].y))); 
        }

        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;

        if (l<=ll0) o.push_back(Point<int>(int(p.x),int(p.y)));

        l+=s2;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++; }
        W.push_back(o);

      }
      i = W.erase(i); i--;
      continue;
    }

    // большой водоем
    if (test_object(*i, "2 * 0 1 33  3  85 * 15 * * * * * * *") ||
        test_object(*i, "3 * 0 1 33  3  85 * 15 * * * * *")) {
      i->area_fill = 20;
      continue;
    }

    // город
    if (test_object(*i, "2 * 0 1  0 27  94 * 15 * * * * * * *") ||
        test_object(*i, "3 * 0 1  0 27  94 * 15 * * * * *")) {
      i->area_fill = 20;
      continue;
    }

    // город
    if (test_object(*i, "4 * 24 55 * 18 * * *")){
      i->pen_color = 0;
      continue;
    }

    // платформа
    if (test_object(*i, "2 * * * 4 * 57 * * * * 0 * * * 1")){
      Point<double> p1((*i)[0].x, (*i)[0].y), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 4 0 * 80 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 4 0 * 80 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig_object o = make_object(*i, "2 1 0 1 0 7 * * 20 * 0 0 0 0 0 *");
      o.clear();
      double l = 80, w=40; // ширина и длина станции
      o.push_back(Point<int>(int(p1.x + v1.x*l + v2.x*w), int(p1.y + v1.y*l + v2.y*w)));
      o.push_back(Point<int>(int(p1.x + v1.x*l - v2.x*w), int(p1.y + v1.y*l - v2.y*w)));
      o.push_back(Point<int>(int(p1.x - v1.x*l - v2.x*w), int(p1.y - v1.y*l - v2.y*w)));
      o.push_back(Point<int>(int(p1.x - v1.x*l + v2.x*w), int(p1.y - v1.y*l + v2.y*w)));
      o.push_back(Point<int>(int(p1.x + v1.x*l + v2.x*w), int(p1.y + v1.y*l + v2.y*w)));
      *i=o; // сюда бы еще поправки на сплайны...
      continue;
    }
    // порог
    if (test_object(*i, "2 * * * 8 * 57 * * * * 0 * * * 1")){
      Point<double> p1((*i)[0].x, (*i)[0].y), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 86 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 86 * * * * * * *")){
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
    // водопад
    if (test_object(*i, "2 * * * 17 * 57 * * * * 0 * * * 1")){
      Point<double> p1((*i)[0].x, (*i)[0].y), p2=p1;
      Point<double> v1, v2;
      if ( W.nearest_pt(v2, p2, "2 * 0 * 33 * 86 * * * * * * * * *") <
           W.nearest_pt(v1, p1, "3 * 0 * 33 * 86 * * * * * * *")){
        v1=v2; p1=p2;
      }
      v2=Point<double>(-v1.y,v1.x);
      fig_object o = make_object(*i, "2 1 0 3 1 0 * * 0 * 0 1 0 0 0 *");
      o.clear();
      double w = 30; // длина штриха
      o.push_back(Point<int>(int(p1.x + v2.x*w),int(p1.y + v2.y*w)));
      o.push_back(Point<int>(int(p1.x - v2.x*w),int(p1.y - v2.y*w)));
      *i=o;
      continue;
    }
    // отметка уреза воды
    if (test_object(*i, "2 * * * 1 * 57 * * * * 1 * 0 0 1")){
      *i = make_object(*i, "1 3 0 1 33 7 57 -1 20 2.000 1 0.000 * * 23 23 * * * *");
      i->center_x = i->start_x = i->end_x = (*i)[0].x;
      i->center_y = i->start_y = i->end_y = (*i)[0].y;
      continue;
    }
    // автобусная остановка
    if (test_object(*i, "2 * * * 4 * 57 * * * * 1 * * * 1")){
      *i = make_object(*i, "4 1 4 55 -1 18 6 0.0000 4");
      (*i)[0].y+=35;
      i->text="A";
      continue;
    }

    // текст, не привязанный к объекту
    if (test_object(*i, "4 1 12 55 -1 3 8 0.0000 4")){
      i->pen_color=0;
      continue;
    }

    // мост
    if (test_object(*i, "2 * * * 7 * 77 * * * * * * 0 0 2")){

        Point<double> p1 ((*i)[0].x, (*i)[0].y);
        Point<double> p2 ((*i)[1].x, (*i)[1].y);
        double ll = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        Point<double> vt ((p1.x-p2.x)/ll, (p1.y-p2.y)/ll);
        Point<double> vn (-vt.y, vt.x);
        if (i->thickness>1) i->thickness+=2;
        double w = (i->thickness)*15/2.0; // ширина моста
        double l = 20.0;                    // длина "стрелок"

        *i=make_object("2 1 0 0 0 7 77 * 20 * 0 0 0 0 0 *");
        i->push_back(Point<int>(int(p1.x + vn.x*w), int(p1.y + vn.y*w)));
        i->push_back(Point<int>(int(p2.x + vn.x*w), int(p2.y + vn.y*w)));
        i->push_back(Point<int>(int(p2.x - vn.x*w), int(p2.y - vn.y*w)));
        i->push_back(Point<int>(int(p1.x - vn.x*w), int(p1.y - vn.y*w)));

        fig_object o = make_object("2 1 0 1 0 7 76 * -1 * 0 0 0 0 0 *");
        o.clear();
        o.push_back(Point<int>( int(p1.x + vn.x*w + (vn.x+vt.x)*l), int(p1.y + vn.y*w + (vn.y+vt.y)*l)));
        o.push_back(Point<int>( int(p1.x + vn.x*w),                 int(p1.y + vn.y*w)));
        o.push_back(Point<int>( int(p2.x + vn.x*w),                 int(p2.y + vn.y*w)));
        o.push_back(Point<int>( int(p2.x + vn.x*w + (vn.x-vt.x)*l), int(p2.y + vn.y*w + (vn.y-vt.y)*l)));
        W.push_back(o);

        o.clear();
        o.push_back(Point<int>( int(p1.x - vn.x*w + (-vn.x+vt.x)*l), int(p1.y - vn.y*w + (-vn.y+vt.y)*l)));
        o.push_back(Point<int>( int(p1.x - vn.x*w),                  int(p1.y - vn.y*w)));
        o.push_back(Point<int>( int(p2.x - vn.x*w),                  int(p2.y - vn.y*w)));
        o.push_back(Point<int>( int(p2.x - vn.x*w + (-vn.x-vt.x)*l), int(p2.y - vn.y*w + (-vn.y-vt.y)*l)));
        W.push_back(o);
    }

    // туннель
    if (test_object(*i, "2 * * * 3 * 77 * * * * * * 0 0 2")){

        Point<double> p1 ((*i)[0].x, (*i)[0].y);
        Point<double> p2 ((*i)[1].x, (*i)[1].y);
        double ll = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        Point<double> vt ((p1.x-p2.x)/ll, (p1.y-p2.y)/ll);
        Point<double> vn (-vt.y, vt.x);
        double l = 20.0;                    // длина "стрелок"

        i->pen_color=0;
        i->depth=81;

        fig_object o = make_object("2 1 0 1 0 7 81 * -1 * 0 0 0 0 0 *");
        o.clear();
        o.push_back(Point<int>(int(p1.x + (vn.x+vt.x)*l),  int(p1.y + (vn.y+vt.y)*l)));
        o.push_back(Point<int>(int(p1.x),                  int(p1.y)));
        o.push_back(Point<int>(int(p1.x + (-vn.x+vt.x)*l), int(p1.y + (-vn.y+vt.y)*l)));
        W.push_back(o);

        o.clear();
        o.push_back(Point<int>(int(p2.x + (vn.x-vt.x)*l),  int(p2.y + (vn.y-vt.y)*l)));
        o.push_back(Point<int>(int(p2.x),                int(p2.y)));
        o.push_back(Point<int>(int(p2.x + (-vn.x-vt.x)*l), int(p2.y + (-vn.y-vt.y)*l)));
        W.push_back(o);
    }

    // кривой текст
    if ((test_object(*i, "2 * * * * * 55 * * * * * * * * *"))||
        (test_object(*i, "3 * * * * * 55 * * * * * * * "))){
      if ((fs<2)||(i->comment.size()<1)||(i->comment[0].size()<1)) continue;
      double l=0;
      vector<double> ls;
      Line<double> vs;
      Line<double> ps;
      ps.push_back(Point<double>((*i)[0].x, (*i)[0].y));
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
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

      double dl=l/i->comment[0].size(); l=dl/2;
      Point<double> p((*i)[0].x, (*i)[0].y);
      p.x-=vs[0].y*shift;
      p.y+=vs[0].x*shift;
      double a = atan2(vs[0].y, vs[0].x);
      int seg=0;

      fig_object o = make_object(*i, "4 1 * * * 3 * * 4");
      o.font_size=i->thickness;

      for (int j=0; j<i->comment[0].size(); j++){
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        a = atan2(vs[seg].y,vs[seg].x);
        p.x = ps[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = ps[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
        p.x-=vs[seg].y*shift;
        p.y+=vs[seg].x*shift;
        o.angle=-a;
        o.clear();
        o.push_back(Point<int>(int(p.x), int(p.y)));
        o.text=i->comment[0][j];
        if (j==0) *i=o; else W.push_back(o);
        l+=dl;
      }
    }

    // ЛЭП
    if ((test_object(*i, "2 * 0 * 35 * 83 * * * * * * * * *"))&&(fs>1)){
      double ll0=0;
      vector<double> ls;
      Line<double> vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }


      fig_object o = make_object(*i, "2 1 0 2 35 7 82 -1 -1 0.000 0 0 -1 1 1 * 0 0 2.00 90.00 90.00 0 0 2.00 90.00 90.00");
      o.farrow_width = (i->thickness<3)? 60:90;
      o.barrow_width = (i->thickness<3)? 60:90;
      o.farrow_height = (i->thickness<3)? 60:90;
      o.barrow_height = (i->thickness<3)? 60:90;
      double w = (i->thickness<3)? 40:60;

      double step = 400;
      double l=0;
      int n=0;
      Point<double> p((*i)[0].x, (*i)[0].y);
      int seg=0;

      while (l<ll0){
        Point<double> v;
        if (n%2 == 0){ o.forward_arrow = 0; o.backward_arrow = 0; v.x=-vs[seg].y; v.y=vs[seg].x;}
        if (n%4 == 1){ o.forward_arrow = 1; o.backward_arrow = 0; v.x=vs[seg].x; v.y=vs[seg].y;}
        if (n%4 == 3){ o.forward_arrow = 0; o.backward_arrow = 1; v.x=vs[seg].x; v.y=vs[seg].y;}
        o.clear();
        o.push_back(Point<int>(int(p.x+v.x*w), int(p.y+v.y*w)));
        o.push_back(Point<int>(int(p.x-v.x*w), int(p.y-v.y*w)));
        W.push_back(o);
        l+=step; n++;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
      }
    }

    // газопровод
    if ((test_object(*i, "2 * 1 * 35 * 83 * * * * * * * * *"))&&(fs>1)){
      double ll0=0;
      vector<double> ls;
      Line<double> vs;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vs.push_back(v);
        }
        else  vs.push_back(Point<double>(1,0));
      }

      i->line_style=0;
      fig_object o = make_object(*i, "1 3 0 1 35 7 82 -1 20 0.000 1 0.0000 * * 40 40 * * * *");

      double step = 600;
      double l=0;
      Point<double> p((*i)[0].x, (*i)[0].y);
      int seg=0;

      while (l<ll0){
        o.center_x = o.start_x = o.end_x = int(p.x);
        o.center_y = o.start_y = o.end_y = int(p.y);
        W.push_back(o);
        l+=step;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
        p.x = (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vs[seg].x;
        p.y = (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vs[seg].y;
      }
    }
    // обрыв
    if ((test_object(*i, "3 * 2 * 18 * 79 * * * * 0 0 *"))||
        (test_object(*i, "2 * 2 * 18 * 79 * * * * * * 0 0 *"))){
      i->thickness=0;
    }
    // обрыв
    if (((test_object(*i, "3 * 0 * 18 * 79 * * * * 0 0 *"))||
         (test_object(*i, "2 * 0 * 18 * 79 * * * * * * 0 0 *")))&&(fs>1)){
      // найдем ближайший низ обрыва, определим, взаимные направления...
      fig_world::const_iterator no, nomin;
      double dmin=1e99, amin;
      int fs1;
      for (no = W.begin(); no != W.end(); no++){
        if (!test_object(*no, "3 * 2 * 18 * 79 * * * * * * *") && 
            !test_object(*no, "2 * 2 * 18 * 79 * * * * * * * * *")) continue;
        fs1=no->size();
        double sum=0;
        int sx=0, sy=0,sxx=0, sxy=0;
        for (int j=0;j<fs;j++){
          double min=1e99; 
          int kmin=0;
          for (int k=0;k<fs1;k++){
            double l = sqrt(((*i)[j].x-(*no)[k].x)*((*i)[j].x-(*no)[k].x) + 
                            ((*i)[j].y-(*no)[k].y)*((*i)[j].y-(*no)[k].y));
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
      Line<double> vsa, vsb, psa, psb;
      fs1=nomin->size();

      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
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
        Point<double> p1 ((*nomin)[j-1].x, (*nomin)[j-1].y);
        Point<double> p2 ((*nomin)[j].x,   (*nomin)[j].y);
        if (amin<0){
          p1 = Point<double>((*nomin)[fs1-j].x,   (*nomin)[fs1-j].y);
          p2 = Point<double>((*nomin)[fs1-j-1].x, (*nomin)[fs1-j-1].y);
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

      i->pen_color=18;
      i->cap_style=1;
      fig_object o = *i;
      o.type=2; o.sub_type=1;
      o.cap_style=0;
      o.clear();

      double step = 50;

      double stepa = l0a/ (l0a+l0b)*2 * step;
      double stepb = l0b/ (l0a+l0b)*2 * step;
      stepa= (l0a-1)/floor(l0a/stepa);
      stepb= (l0b-1)/floor(l0b/stepb);

      double la=0, lb=0;
      Point<double> pa(psa[0]);
      Point<double> pb(psb[0]);
      int sega=0, segb=0;

      while ((la<=l0a)&&(lb<=l0b)){
        o.clear();
        o.push_back(Point<int>(int(pa.x),int(pa.y)));
        o.push_back(Point<int>(int(pb.x),int(pb.y)));
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

    // обрыв
    if ((test_object(*i, "2 * 0 * 18 * 79 * * * * * * * * *")||
         test_object(*i, "3 * 0 * 18 * 79 * * * * * * *"))&&(fs>1)){
      int k=0;
      if (i->backward_arrow==1) k=-1;
      if (i->forward_arrow==1) k=1;
      if (k==0) continue;
      double ll0=0;
      vector<double> ls;
      Line<double> vt;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vt.push_back(v);
        }
        else  vt.push_back(Point<double>(1,0));
      }

      i->pen_color=18;
      i->forward_arrow=0;
      i->backward_arrow=0;
      i->cap_style=1;
      fig_object o = *i;
      o.type=2;
      o.sub_type=1;

      double step = 40;
      step= ll0/floor(ll0/(step+1));
      double w = 15;

      double l=w/2;
      int seg=0;

      while (l+w/2<ll0){
        Point<double> p(
         (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vt[seg].x,
         (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vt[seg].y);
        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));
        o.push_back(Point<int>(int(p.x-k*vt[seg].y*w),int(p.y+k*vt[seg].x*w)));
        W.push_back(o);
        l+=step;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
      }
    }

    // кладбище
    if (test_object(*i, "2 * 0 1  0 32  92 *  5 * * * * * * *")){
      i->area_fill=10;
      int w1=23, w2=45;

      int minx=0x7FFFFFFF, maxx=-0x7FFFFFFF;
      int miny=0x7FFFFFFF, maxy=-0x7FFFFFFF;
      for (int j=0; j<fs; j++){ 
        if ((*i)[j].x>maxx) maxx=(*i)[j].x;
        if ((*i)[j].x<minx) minx=(*i)[j].x;
        if ((*i)[j].y>maxy) maxy=(*i)[j].y;
        if ((*i)[j].y<miny) miny=(*i)[j].y;
      }
      if ((minx>maxx)||(miny>maxy)) continue;

      int x=(maxx+minx)/2, y=(maxy+miny)/2-w1/2;

      fig_object o = make_object("2 1 0 1 0 * 57 * * * * 0 * * * *");
      o.push_back(Point<int>(x-w1,y));
      o.push_back(Point<int>(x+w1,y));
      W.push_back(o); 
      o.clear();
      o.push_back(Point<int>(x,y-w1));
      o.push_back(Point<int>(x,y+w2));
      W.push_back(o);
    }

    // забор
    if (test_object(*i, "2 * 0 * 20 * 81 * * * * * * * * *")&&(fs>1)){
      int k=0;
      if (i->backward_arrow==1) k=-1;
      if (i->forward_arrow==1) k=1;
      if (k==0) continue;
      double ll0=0;
      vector<double> ls;
      Line<double> vt;
      for (int j=1; j<fs; j++){
        Point<double> p1 ((*i)[j-1].x, (*i)[j-1].y);
        Point<double> p2 ((*i)[j].x, (*i)[j].y);
        double dl = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
        ll0+=dl; ls.push_back(ll0);
        if (p1!=p2){
          Point<double> v ((p2.x-p1.x)/dl, (p2.y-p1.y)/dl);
          vt.push_back(v);
        }
        else  vt.push_back(Point<double>(1,0));
      }

      i->pen_color=0;
      i->forward_arrow=0;
      i->backward_arrow=0;
      i->cap_style=1;
      i->type=2;
      i->sub_type=1;
      fig_object o = *i;

      double step = 150;
      double w    = 30;

      double l=w;
      int seg=0;

      while (l+w<ll0){
        Point<double> p(
         (*i)[seg].x + (l - (seg==0? 0: ls[seg-1]))*vt[seg].x,
         (*i)[seg].y + (l - (seg==0? 0: ls[seg-1]))*vt[seg].y);
        Point<double> vn(-vt[seg].y, vt[seg].x);
        o.clear();
        o.push_back(Point<int>(int(p.x),int(p.y)));
        o.push_back(Point<int>(int(p.x+k*(vn.x-vt[seg].x)*w),
                               int(p.y+k*(vn.y-vt[seg].y)*w)));
        W.push_back(o);
        o[0].x+=int(vt[seg].x*w);
        o[0].y+=int(vt[seg].y*w);
        o[1].x+=int(vt[seg].x*w);
        o[1].y+=int(vt[seg].y*w);
        W.push_back(o);
        l+=step;
        while ((ls[seg]<=l)&&(seg<ls.size())){ seg++;}
      }
    }



  }
//  text_bbxs(W);
  write(out, W);
}
