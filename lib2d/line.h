#ifndef LINE_H
#define LINE_H

#include <boost/operators.hpp>
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
    typename Line<T>::const_iterator i1=this->begin(), i2=p.begin();
    do {
      if (i1==this->end()){
        if (i2==p.end()) return false;
        else return true;
      }
      else if (i2==p.end()) return false;

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

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & l){
  s << "Line(";
  for(typename Line<T>::const_iterator i=l.begin(); i!=l.end(); i++) 
    s << "(" << i->x << "," << i->y << ")";
  s << ")\n";
  return s;
}

#endif /* LINE_H */
