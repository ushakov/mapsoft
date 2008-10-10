#ifndef IO2D_H
#define IO2D_H

#include "point.h"
#include "line.h"
#include "rect.h"

#include <iostream>
#include <iomanip>

#define MAXLEN 4096

template <typename T>
std::ostream & operator<< (std::ostream & s, const Point<T> & p){
  s << p.x << "," << p.y;
  return s;
}

template <typename T>
std::ostream & operator<< (std::ostream & s, const Line<T> & l){
  for (typename Line<T>::const_iterator i=l.begin(); i!=l.end(); i++)
    s << ((i==l.begin())? "":",") << *i;
  return s;
}

template <typename T>
std::ostream & operator<< (std::ostream & s, const Rect<T> & r){
  s << r.w << "x" << r.h << std::showpos << r.x << r.y << std::noshowpos;
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Point<T> & p){
  char sep;
  s >> p.x >> sep;
  if (sep!=',') return s;
  s >> p.y;
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Line<T> & l){
  Point<T> p;
  char sep;
  s >> p;
  l.push_back(p);
  while (!s.eof()){
    s >> sep >> p;
    if (sep!=',') return s;
    l.push_back(p);
  }
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Rect<T> & r){
  char sep;

  s >> r.w >> sep >> r.h;
  if (sep!='x') return s;

  s >> sep >> r.x;
  if (sep=='-') r.x=-r.x; 
  else if (sep!='+') return s;

  s >> sep >> r.y;
  if (sep=='-') r.y=-r.y; 
  else if (sep!='+') return s;

  return s;
}

#endif /* IO2D_H */
