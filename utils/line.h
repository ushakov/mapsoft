#ifndef LINE_H
#define LINE_H

#include <boost/operators.hpp>
//#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include "point.h"

// ����� ��� ������� �����

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

  // ����� ������, ���� ������ ������������ ����� ������,
  // ��� �� ����������
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

  // ����� �� ��������, ��� ==, �� ��� ����� ������ ���������...
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

  // ���������, �� ��������� �� ����� � ����� l ������� �� ��������� ������
  // (������ ������������ � shift)
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


};

// ���������� ����� � ����, ���� �� ����� ����� e
void merge(std::list<Line<double> > & lines, double e);

// ��������� ����� �� ���������, ������ �� ����� points �����
void split (std::list<Line<double> > & lines, int points);

// ������ �� ����� ��������� �����, ���, ����� �����
// �� ���������� �� ��������� ��������� ����� ��� �� e
void generalize (std::list<Line<double> > & lines, double e);

// �������� ��� �����, ���������� �� ������������� cutter 
void crop_lines(std::list<Line<double> > & lines, const Line<double> & cutter);

// ��������� ����� �� ���� a ������ ����� p0.
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
