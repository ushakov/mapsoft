#ifndef POINT_CROSS_H
#define POINT_CROSS_H

#include "point.h"

// пересечение прямых, задаваемых отрезками p1-p2 и p3-p4.
// для точки пересечения p возвращаются два числа:
// |p-p1|/|p2-p1| и |p-p3|/|p4-p3|
// если прямые параллельны - возникает исключение.

template <typename T>
Point<double> find_cross_ab(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<T> p12 = p2-p1;
  Point<T> p34 = p4-p3;
  Point<T> p13 = p3-p1;
  double k = p12.x*p34.y - p12.y*p34.x;
  double ka = p13.x*p34.y - p13.y*p34.x;
  double kb = p13.x*p12.y - p13.y*p12.x;
  if (k==0) throw 0; // параллельные линии(?)
  return Point<double>(ka/k, kb/k);
}

// Пересечение двух отрезков
// Отрезок включает первую точку, но не включает последнюю
// Если отрезки не пересекаются - возникает исключение

template <typename T>
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<double> ab = find_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // пересечение - за пределами отрезка
  if ((ab.y<0)||(ab.y>=1)) throw 0; // пересечение - за пределами отрезка
  return Point<T>((T)(p1.x+ab.x*(p2.x-p1.x)), (T)(p1.y+ab.x*(p2.y-p1.y)));
}


#endif /* POINT_CROSS_H */
