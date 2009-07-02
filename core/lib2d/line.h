#ifndef LINE_H
#define LINE_H

#include <boost/operators.hpp>
#include <iostream>
#include <ios>
#include <cmath>
#include <list>
#include <vector>
#include "point.h"
#include "point_utils.h"
#include "rect.h"


/** Ломаная линия (std::vector<Point<T> >). */

template <typename T>
struct Line : std::vector<Point<T> >
#ifndef SWIG
    , public boost::additive<Line<T> >,
    public boost::additive<Line<T>, Point<T> >,
    public boost::multiplicative<Line<T>,T>,
    public boost::less_than_comparable<Line<T> >,
    public boost::equality_comparable<Line<T> >
#endif
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
    for (int i=0; i<this->size()-1; i++)
      ret+=pdist((*this)[i], (*this)[i+1]);
    return ret;
  }

#ifndef SWIG
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
#endif

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

  // invert line
  Line<T> inv(void) const{
    Line<T> ret;
    for (typename Line<T>::const_reverse_iterator i=this->rbegin();
                              i!=this->rend(); i++) ret.push_back(*i);
    return ret;
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

#ifndef SWIG
  operator Line<double>() const{
    Line<double> ret;
    for (typename Line<T>::const_iterator i=this->begin(); i!=this->end(); i++)
      ret.push_back(dPoint(*i));
    return ret;
  }

  operator Line<int>() const{
    Line<int> ret;
    for (typename Line<T>::const_iterator i=this->begin(); i!=this->end(); i++)
      ret.push_back(iPoint(*i));
    return ret;
  }
#endif  // SWIG
#ifdef SWIG
  %extend {
    Line<T> operator+ (Point<T> &p) { return *$self + p; }
    Line<T> operator- (Point<T> &p) { return *$self - p; }
    Line<T> operator* (T p) { return *$self * p; }
    Line<T> operator/ (T p) { return *$self / p; }
    swig_cmp(Line<T>);
    swig_str();
  }
#endif  // SWIG
};

typedef Line<double> dLine;
typedef Line<int>    iLine;

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & l){
  for (typename Line<T>::const_iterator i=l.begin(); i!=l.end(); i++)
    s << ((i==l.begin())? "":",") << *i;
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Line<T> & l){
  Point<T> p;
  char sep;
  s >> p;
  l.push_back(p);
  while (!s.eof()){
    s >> std::ws >> sep >> std::ws >> p;
    if (sep!=','){
      s.setstate(std::ios::failbit);
      return s;
    }
    l.push_back(p);
  }
  s.setstate(std::ios::goodbit);
  return s;
}

// Line with multiple segments (std::vector<Line<T> >)

template <typename T>
struct MultiLine : std::vector<Line<T> >
#ifndef SWIG
    , public boost::additive<MultiLine<T> >,
    public boost::additive<MultiLine<T>, Point<T> >,
    public boost::multiplicative<MultiLine<T>,T>
#endif  //SWIG
{

  MultiLine<T> & operator/= (T k) {
    for (typename MultiLine<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)/=k;
    return *this;
  }

  MultiLine<T> & operator*= (T k) {
    for (typename MultiLine<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)*=k;
    return *this;
  }

  MultiLine<T> & operator+= (Point<T> p) {
    for (typename MultiLine<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)+=p;
    return *this;
  }

  MultiLine<T> & operator-= (Point<T> p) {
    for (typename MultiLine<T>::iterator i=this->begin(); i!=this->end(); i++) (*i)-=p;
    return *this;
  }

  double length () const {
    double ret=0;
    for(typename MultiLine<T>::const_iterator i=this->begin(); i!=this->end(); i++)
      ret+=i->length();
    return ret;
  }

  Rect<T> range() const{
    if (this->size()<1) return Rect<T>(0,0,0,0);
    typename MultiLine<T>::const_iterator i=this->begin();
    Rect<T> ret=i->range();
    while ((++i) != this->end())  ret = rect_bounding_box(ret, i->range());
    return ret;
  }

  Point<T> center() const{
    Rect<T> r=range();
    return (r.TLC() + r.BRC())/2;
  }

#ifndef SWIG
  operator MultiLine<double>() const{
    MultiLine<double> ret;
    for (typename MultiLine<T>::const_iterator i=this->begin(); i!=this->end(); i++)
      ret.push_back(dLine(*i));
    return ret;
  }

  operator MultiLine<int>() const{
    MultiLine<int> ret;
    for (typename MultiLine<T>::const_iterator i=this->begin(); i!=this->end(); i++)
      ret.push_back(iLine(*i));
    return ret;
  }
#else  //SWIG
  %extend {
    MultiLine<T> operator+ (Point<T> &p) { return *$self + p; }
    MultiLine<T> operator- (Point<T> &p) { return *$self - p; }
    MultiLine<T> operator* (T p) { return *$self * p; }
    MultiLine<T> operator/ (T p) { return *$self / p; }
    swig_cmp(MultiLine<T>);
  }
#endif  //SWIG  
};

typedef MultiLine<double> dMultiLine;
typedef MultiLine<int>    iMultiLine;

#endif /* LINE_H */
