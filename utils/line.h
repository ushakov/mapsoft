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
  
  Line<T> & operator+= (Line const & t)
  {
	x += t.x;
	y += t.y;
	return *this;
  }

  Line<T> & operator/= (T k) {
    for(typename Line<T>::iterator i=begin(); i!=end(); i++) (*i)/=k;
    return *this;
  }

  Line<T> & operator*= (T k) {
    for(typename Line<T>::iterator i=begin(); i!=end(); i++) (*i)*=k;
    return *this;
  }

  double length () const {
    double ret=0;
    for(typename Line<T>::iterator i=begin(); i!=end(); i++) 
      ret+=sqrt(i->x*i->x + i->y*i->y);
    return ret;
  }

  // линия меньше, если первая отличающаяся точка меньше,
  // или не существует
  bool operator< (const Line<T> & p) const {
    Line<T>::iterator i1=begin(), i2=p.begin();
    do {
      if (i1==end()){
        if (i2!=p.end()) return true;
        else return false;
      }
      if ((*i1)!=(*i2)) return (*i1) < (*i2);
      i1++; i2++;
    } while(1);
  }

  bool operator== (const Line<T> & p) const {
    if (size()!=p.size()) return false;
    Line<T>::iterator i1=begin(), i2=p.begin();
    do {
      if (i1==end()) return true;
      if ((*i1)!=(*i2)) return false;
      i1++; i2++;
    } while(1);
  }
};

// склеивание линий в одну, если их концы ближе e
std::vector<Line<double> > merge (const std::vector<Line<double> >& lines, double e){
  for (std::vector<Line<double> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
  for (std::vector<Line<double> >::iterator i2 = i1; i2!=lines.end(); i2++){
    if (i1==i2) continue;
    if (pdist(i1->begin(),i2->begin())<e) {i1.push_back(i2.begin()+1, i2.end()); lines.erase(i2); i2=i1; continue;}
    if (pdist(i1->begin(),i2->rbegin())<e) {i1.push_back(i2.rbegin()+1, i2.rend()); lines.erase(i2); i2=i1; continue;}
    if (pdist(i1->rbegin(),i2->begin())<e) {i1.insert(i2.begin(), i2.rbegin()+1, i2.rend()); lines.erase(i2); i2=i1; continue;}
  }
  }
}

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & p)
{
  s << "Line(";
  for(typename Line<T>::iterator i=begin(); i!=end(); i++) 
    s << "(" << i->x << "," << i->y << ")";
  s << ")\n";
  return s;
}

#endif /* LINE_H */
