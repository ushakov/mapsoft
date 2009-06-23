#ifndef POINT_UTILS_H
#define POINT_UTILS_H

#include <cmath>
#include "point.h"

template <typename T>
double pscal(const Point<T> & p1, const Point<T> & p2){
  return p1.x*p2.x + p1.y*p2.y;
}
template <typename T>
double pdist(const Point<T> & p){
  return sqrt((double)(pscal(p,p)));
}
template <typename T>
double pdist(const Point<T> & p1, const Point<T> & p2){
  return pdist(p1-p2);
}
template <typename T>
dPoint pnorm(const Point<T> & p){
  double l = pdist(p);
  return dPoint(double(p.x)/l, double(p.y/l));
}

#endif /* POINT_UTILS_H */
