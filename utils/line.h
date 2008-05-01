#ifndef LINE_H
#define LINE_H

#include <boost/operators.hpp>
//#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include "point.h"
#include "rect.h"

/** Ломаная линия (std::vector<Point<T> >). */

template <typename T> 
struct Line
  : public boost::additive<Line<T> >,
    public boost::additive<Line<T>, Point<T> >,
    public boost::multiplicative<Line<T>,T>,
    public boost::less_than_comparable<Line<T> >,
    public boost::equality_comparable<Line<T> >,
    std::vector<Point<T> >
{
  
  Line<T> & operator/= (T k) {
    for (typename Line<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)/=k;
    return *this;
  }

  Line<T> & operator*= (T k) {
    for (typename Line<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)*=k;
    return *this;
  }

  Line<T> & operator+= (Point<T> p) {
    for (typename Line<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)+=p;
    return *this;
  }

  Line<T> & operator-= (Point<T> p) {
    for (typename Line<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)-=p;
    return *this;
  }


  double length () const {
    double ret=0;
    for(typename Line<T>::iterator i=this->begin(); i!=this->end(); i++) 
      ret+=sqrt(i->x*i->x + i->y*i->y);
    return ret;
  }

  // линия меньше, если первая отличающаяся точка меньше,
  // или не существует
  bool operator< (const Line<T> & p) const {
    typename Line<T>::iterator i1=this->begin(), i2=p.begin();
    do {
      if (i1==this->end()){
        if (i2!=p.end()) return true;
        else return false;
      }
      if ((*i1)!=(*i2)) return (*i1) < (*i2);
      i1++; i2++;
    } while(1);
  }

  bool operator== (const Line<T> & p) const {
    if (this->size()!=p.size()) return false;
    typename Line<T>::const_iterator i1=this->begin(), i2=p.begin();
    do {
      if (i1==this->end()) return true;
      if ((*i1)!=(*i2)) return false;
      i1++; i2++;
    } while(1);
  }

  // такая же проверка, как ==, но для линий идущих навстречу...
  bool isinv(const Line<T> & p) const {
    if (this->size()!=p.size()) return false;
    typename Line<T>::const_iterator i1=this->begin();
    typename Line<T>::const_reverse_iterator  i2=p.rbegin();
    do {
      if (i1==this->end()) return true;
      if ((*i1)!=(*i2)) return false;
      i1++; i2++;
    } while(1);
  }

  // проверить, не переходит ли линия в линию l сдвигом на некоторый вектор
  // (вектор записывается в shift)
  bool isshifted(const Line<T> & l, Point<T> & shift) const{
    shift = Point<T>(0,0);
    if (this->size()!=l.size()) return false;
    if (this->size()==0) return true;
    typename Line<T>::const_iterator i1=this->begin(), i2=l.begin();
    shift = (*i2) - (*i1);
    do {
      if (i1==this->end()) return true;
      if ((*i2)-(*i1) != shift) return false;
      i1++; i2++;
    } while(1);
  }

  Rect<T> range() const{
    if (this->size()<1) return Rect<T>(0,0,0,0);
    Point<T> min((*this)[0]), max((*this)[0]);

    for (typename Line<T>::const_iterator i = this->begin(); i!=this->end(); i++){
      if (i->x > max.x) max.x = i->x;
      if (i->y > max.y) max.y = i->y;
      if (i->x < min.x) min.x = i->x;
      if (i->y < min.y) min.y = i->y;
    }
    return Rect<T>(min,max);
  }

  Point<T> center() const{
    Rect<T> r=range();
    return (r.TLC() + r.BRC())/2;
  }


};

// склеивание линий в одну, если их концы ближе e
void merge(std::list<Line<double> > & lines, double e);

// разбиение линии на несколько, каждая не более points точек
void split (std::list<Line<double> > & lines, int points);

// Убрать из линии некоторые точки, так, чтобы линия
// не сместилась от исходного положения более чем на e
void generalize (std::list<Line<double> > & lines, double e);

//// обрезать все линии, вылезающие за многоугольник cutter 
//void crop_lines(std::list<Line<double> > & lines, const Line<double> & cutter);

// обрезать все линии, входящие/не входящие в многоугольник cutter и добавить их в lines1
//void crop_lines(std::list<Line<double> > & lines, std::list<Line<double> > & lines1, const Line<double> & cutter, bool cutouter);

// лежит ли точка в многоугольнике poly
template<typename T>
bool test_pt (const Point<double> & pt, const T & poly){
  double a = 0;
  typename T::const_iterator p1,p2;
  for (int i = 0; i<poly.size(); i++){
    Point<double> v1 = Point<double>(poly[i]) - pt;
    Point<double> v2 = Point<double>(poly[(i+1)%poly.size()]) - pt;
    double dd = pdist(v1)*pdist(v2);
    if (dd==0) return true;

    double s = v1.x*v2.y - v1.y*v2.x;
    double c = pscal(v1,v2)/dd;
    if (fabs(c)>=1) {continue;}

    if (s<0) a+=acos(c); else a-=acos(c);
  }

  return (fabs(a)>M_PI);
}

// обрезать все линии, входящие/не входящие в многоугольник cutter и добавить их в lines1
template<typename T>
void crop_lines(std::list<T> & lines,
                std::list<T> & lines1,
                const T & cutter, bool cutouter){
  for (int j = 0; j<cutter.size(); j++){
    for (typename std::list<T>::iterator l = lines.begin(); l!=lines.end(); l++){
      for (int i = 0; i<l->size()-1; i++){
        Point<double> pt;
        try { pt = find_cross((*l)[i], (*l)[i+1], cutter[j], cutter[(j+1)%cutter.size()]); }
        catch (int n) {continue;}
        // разбиваем линию на две, уже обработанный кусок помещаем перед l
        T l1(*l); l1.clear(); // мы хотим работать не только с линиями, но и с потомками!
        for (int k=0; k<=i; k++) l1.push_back((*l)[k]);
        l1.push_back(pt);
        lines.insert(l, l1);
        // из *l стираем все точки до i-1-й
        l->erase(l->begin(), l->begin()+i);
        *(l->begin()) = pt;
        // продолжаем со второго звена оставшейся линии
        i=1;
      }
    }
  }
  // теперь переместим те линии, которые не попадают в нужный район
  typename std::list<T>::iterator l=lines.begin();
  while (l!=lines.end()){
    if (l->size()==0) {l=lines.erase(l); continue;}
    // посчитаем число точек внутри - число точек вне.
    int sum=0;
    for (int i = 1; i<l->size(); i++){
      sum+= (cutouter xor test_pt(((*l)[i]+(*l)[i-1])/2, cutter))? -1:1;
    }
    if (sum<0) {lines1.push_back(*l);  l=lines.erase(l); continue;}
    l++;
  }
}
// ==================


//преобразовать прямоугольник в линию из 4 точек
template<typename T>
Line<T> rect2line(const Rect<T> & r){
  Line<T> ret;
  ret.push_back(r.TLC());
  ret.push_back(r.TRC());
  ret.push_back(r.BRC());
  ret.push_back(r.BLC());
  return  ret;
}

// повернуть линии на угол a вокруг точки p0.
template<typename T>
void lrotate(std::list<Line<double> > & lines, const double a, const Point<T> & p0 = Point<T>(0,0)){
  double c = cos(a);
  double s = sin(a);
  for (typename std::list<Line<T> >::iterator l = lines.begin(); l!=lines.end(); l++){
    for (typename Line<T>::iterator p = l->begin(); p!=l->end(); p++){
      double x = p->x-p0.x, y = p->y-p0.y;
      p->x = x*c - y*s + p0.x;
      p->y = x*s + y*c + p0.y;
    }
  }
}

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & l){
  s << "Line(";
  for(typename Line<T>::const_iterator i=l.begin(); i!=l.end(); i++) 
    s << "(" << i->x << "," << i->y << ")";
  s << ")\n";
  return s;
}


#endif /* LINE_H */
