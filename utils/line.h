#ifndef LINE_H
#define LINE_H

#include <boost/operators.hpp>
//#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "point.h"

// Класс для ломаной линии

template <typename T> 
struct Line
  : public boost::additive<Line<T> >,
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
    typename Line<T>::iterator i1=this->begin(), i2=p.begin();
    do {
      if (i1==this->end()) return true;
      if ((*i1)!=(*i2)) return false;
      i1++; i2++;
    } while(1);
  }
};

// склеивание линий в одну, если их концы ближе e
std::vector<Line<double> > merge (std::vector<Line<double> > lines, double e);

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & p)
{
  s << "Line(";
  for(typename Line<T>::iterator i=p.begin(); i!=p.end(); i++) 
    s << "(" << i->x << "," << i->y << ")";
  s << ")\n";
  return s;
}

#endif /* LINE_H */
