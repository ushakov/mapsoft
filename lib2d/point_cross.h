#ifndef POINT_CROSS_H
#define POINT_CROSS_H

#include "point.h"
#include "point_utils.h"

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

  T k  =  p12.x*p34.y - p12.y*p34.x;
  T ka =  p13.x*p34.y - p13.y*p34.x;
  T kb =  p13.x*p12.y - p13.y*p12.x;

  if (k==0) throw 0; // параллельные линии(?)
  return Point<double>(ka/(double)k, kb/(double)k);
}

// Пересечение двух отрезков
// Отрезок включает первую точку, но не включает последнюю
// Если отрезки не пересекаются - возникает исключение

// TODO: обрабатывать специально случай отрезков, лежащих на одной прямой!
// сейчас они считаются непересекающимися из-за чего возникают ошибки при
// обрезании линий по произвольному многоугольнику!
// (например, плохо работает srtm2fig

template <typename T>
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<double> ab = find_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // пересечение - за пределами отрезка
  if ((ab.y<0)||(ab.y>=1)) throw 0; // пересечение - за пределами отрезка
  return Point<T>(Point<double>(p1)+ab.x*Point<double>(p2-p1));
}

// Кратчайшее расстояние между точкой и отрезком
template <typename T>
double find_dist(const Point<T> & p,
                 const Point<T> & p1, const Point<T> & p2){

  double  ll = pdist(p1,p2);
  if (ll==0) return pdist(p,p1); // отрезок нулевой длины

  Point<double> vec = Point<double>(p2-p1)/ll;

  double l1 = pdist(p,p1);
  double l2 = pdist(p,p2);
  double ret = l1<l2 ? l1:l2;

  double prl = pscal(Point<double>(p-p1), vec);

  if ((prl>=0)&&(prl<=ll)){ // проекция попала на отрезок
    Point<double> pc = Point<double>(p1) + vec * prl;
    double lc=pdist(Point<double>(p),pc);
    ret= ret<lc ? ret:lc;
  }
  return ret;
}

// Кратчайшее расстояние между двумя отрезками
template <typename T>
double find_dist(const Point<T> & p1, const Point<T> & p2,
                 const Point<T> & p3, const Point<T> & p4){
  return min( min(find_dist(p1,p3,p4), find_dist(p2,p3,p4)),
              min(find_dist(p3,p1,p2), find_dist(p4,p1,p2)));
}

#endif /* POINT_CROSS_H */
