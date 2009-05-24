#include "fig_utils.h"
#include "../lib2d/line_rectcrop.h"
#include "../lib2d/point_utils.h"

namespace fig {
using namespace std;

/****************************************************************/

double nearest_pt(Point<double> & vec, Point<double> & pt,  
  const std::list<fig_object> & objects, const std::string & mask, double maxdist){

  Point<double> minp(pt),minvec(1,0);
  double minl=maxdist; // далеко объекты не уносим!!!

  for (fig_world::const_iterator i  = objects.begin(); i != objects.end(); i++){
    if (!test_object(*i, mask)) continue;

    int np = i->size();

    for (int j=1; j<np; j++){
      Point<double> p1((*i)[j-1].x, (*i)[j-1].y);
      Point<double> p2((*i)[j].x, (*i)[j].y);
      double  ll = sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
      Point<double> vec((p2.x-p1.x)/ll, (p2.y-p1.y)/ll);

      double ls = sqrt((pt.x-p1.x)*(pt.x-p1.x)+(pt.y-p1.y)*(pt.y-p1.y)); 
      double le = sqrt((pt.x-p2.x)*(pt.x-p2.x)+(pt.y-p2.y)*(pt.y-p2.y)); 

      if (ls<minl){ minl=ls; minp=p1; minvec=vec; }
      if (le<minl){ minl=le; minp=p2; minvec=vec; }

      double prl = ((pt.x-p1.x)*vec.x+(pt.y-p1.y)*vec.y); 

      if ((prl>=0)&&(prl<=ll)) { // проекция попала на отрезок
        Point<double> p = p1 + vec * ((pt.x-p1.x)*vec.x+(pt.y-p1.y)*vec.y);
        double lc=sqrt((pt.x-p.x)*(pt.x-p.x)+(pt.y-p.y)*(pt.y-p.y));
        if (lc<minl) {
          minl=lc; minp=p; minvec=vec;
        }
      }
    }
  }
  pt=minp;
  vec=minvec;
  return minl;
}

// размер fig-объектов
Rect<int> range(std::list<fig_object> & objects){
  if ((objects.size()<1) || (objects.begin()->size()<1)) return Rect<int>(0,0,0,0);
  int minx=(*objects.begin())[0].x;
  int maxx=(*objects.begin())[0].x;
  int miny=(*objects.begin())[0].y;
  int maxy=(*objects.begin())[0].y;
  for (std::list<fig_object>::const_iterator
       i = objects.begin(); i != objects.end(); i++){
     if (i->type == 1){
       int rx = i->radius_x;
       int ry = i->radius_y;
       int cx = i->center_x;
       int cy = i->center_y;
       if (minx > cx-rx) minx = cx-rx;
       if (maxx < cx+rx) maxx = cx+rx;
       if (miny > cy-ry) miny = cy-ry;
       if (maxy < cy+ry) maxy = cy+ry;
     } else {
       for (int j = 0; j < i->size(); j++){
         int x = (*i)[j].x;
         int y = (*i)[j].y;
         if (minx > x) minx = x;
         if (maxx < x) maxx = x;
         if (miny > y) miny = y;
         if (maxy < y) maxy = y;
      }
    }
  }
  return Rect<int>(Point<int>(minx,miny), Point<int>(maxx,maxy));
}


// заключить fig-объекты в составной объект.
void fig_make_comp(std::list<fig_object> & objects){
  if ((objects.size()<1) || (objects.begin()->size()<1)) return;
  Rect<int> r = range(objects);

  fig_object o;
  o.type=6;
  o.push_back(r.TLC());
  o.push_back(r.BRC());
  objects.insert(objects.begin(), o);
  o.type = -6;
  objects.insert(objects.end(), o);
}

// повернуть на угол a вокруг точки p0
void fig_rotate(std::list<fig_object> & objects, const double a, const Point<int> & p0){
  double c = cos(a);
  double s = sin(a);
  for (std::list<fig_object>::iterator l = objects.begin(); l!=objects.end(); l++){
    for (fig_object::iterator p = l->begin(); p!=l->end(); p++){
      double x = p->x-p0.x, y = p->y-p0.y;
      p->x = x*c - y*s + p0.x;
      p->y = x*s + y*c + p0.y;
    }
    if ((l->type == 4)||(l->type==1)) {
      l->angle += a;
      while (l->angle>M_PI) l->angle-=2*M_PI;
    }
    if (l->type == 1) {
      double x = l->center_x-p0.x, y = l->center_y-p0.y;
      l->center_x = x*c - y*s + p0.x;
      l->center_y = x*s + y*c + p0.y;
    }
  }
}

// преобразовать ломаную в сплайн
void any2xspl(fig_object & o, const double x, const double y){
  if (o.size()<3) return;
  if (o.is_polyline()){
    if      (o.sub_type==1) o.sub_type = 4;
    else if (o.sub_type==3) {
      //в замкнутой ломаной последняя точка совпадает с первой
      //в замкнутом сплайне - нет!
      if (o[0]==o[o.size()-1]) o.resize(o.size()-1);
      o.sub_type = 5;
    }
    else return;
  }
  else if (o.is_spline()) o.sub_type = o.sub_type%2+4;
  else return;

  // добавим точки в соответствии с параметром y
  // Если звено ломаной длиннее, чем 3y, добавим две точки на
  // расстоянии y от края.
  if (y>0){
    fig_object::iterator i=o.begin(), j=i;
    j++; // у нас >3 точек, т.ч. все хорошо
    do{
      double l = pdist(*i,*j);
      if (l > 3*y){
        Point<double> p = pnorm(*j-*i) * y;
        j=o.insert(j, *i + Point<int>(p) ); j++;
        j=o.insert(j, *j - Point<int>(p) ); j++;
      }
      i=j; j++;
    } while (j!=o.end()); 
  }

  o.type = 3;
  o.f.clear();
  for (int j=0; j<o.size(); j++) o.f.push_back(x);
  if (o.sub_type==4){
    o.f[0]=0;
    o.f[o.size()-1]=0;
  }
  return;
}

/// обрезка fig-файла по прямоугольнику
void rect_crop(const Rect<int> & cutter, std::list<fig_object> & objects){
  std::list<fig_object>::iterator o = objects.begin();
  while (o!=objects.end()){
    Line<int> l = *o;
    ::rect_crop(cutter, l, o->is_closed() || (o->area_fill != -1));
    o->set_points(l);
    if (o->size() == 0) o=objects.erase(o);
    else o++;
  }
}

/// Обрезка fig-объекта по прямоугольнику. Объект после обрезки.
/// может не содержать точек, т.е. быть плохим с т.з. fig-а!
void rect_crop(const Rect<int> & cutter, fig_object & o){
  Line<int> l = o;
  ::rect_crop(cutter, l, o.is_closed() || (o.area_fill != -1));
  o.set_points(l);
}


} // namespace
