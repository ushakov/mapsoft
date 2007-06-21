#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

#include "point.h"

// Линия 

template <typename T> 
struct Line : std::vector<Point<T> > {
};


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


template <typename T> 
struct PolyLine : std::list<Line<T> > {};

// Обобщенный многоугольник:
// Состоит из множества замкнутых контуров. 

// Контур с самопересечениями разбивается на несколько контуров.

// Точка плоскости принадлежит многоугольнику, если ее охватывает 
// нечетное число контуров

template <typename T> 
struct Polygon : private std::list<Line<T> > {

    // пересечение - два "адреса": итератор предыдущей точки + расстояние от нее,
    // на двух ветках
    struct crossing_t{ 
      typename Line<T>::iterator i1;
      typename Line<T>::iterator i2;
      double a1, a2;
	crossing_t(const typename Line<T>::iterator & _i1, 
                 const typename Line<T>::iterator & _i2, 
                 const double _a1, const double _a2): i1(_i1), i2(_i2), a1(_a1), a2(_a2){};
	crossing_t swap() const{ return crossing_t(i2,i1,a2,a1); }
    };


    // Добавление контура.
    void add(Line<T> l){
      // Если в контуре меньше трех точек - нам не нужен такой контур
      if (l.size()<3) return;
      // Последняя точка должна иметь те же координаты, что и первая.
      // (Если нет - добавим такую точку)
      if (*l.begin() != *l.rbegin()) l.push_back(*l.begin());

      // Контур с самопересечениями нужно разбить на несколько.

      // Разыщем все пересечения.
      // Для каждого звена ищем пересечения cо всеми следующими звеньями.
      // здесь же проверяем, что точка - это именно пересечение, а не касание
      std::list<crossing_t> crossings;

      typename Line<T>::iterator pa1=l.end(),pa2=l.begin(), pa3=pa2; pa3++;
      while (pa3!=l.end()){ 
        typename Line<T>::iterator pb1=pa2, pb2=pa3, pb3=pb2; pb3++;
          while(pb3!=l.end()){
	  try {
            Point<double> ab = find_cross_ab(*pa2,*pa3,*pb2,*pb3);
	    if ((ab.x<0)||(ab.x>=1)||(ab.y<0)||(ab.y>=1)) throw 0;

	    // найдем 4 направления от точки пересечения (единичные векторы):
            Point<double> va1 = pnorm(*pa3-*pa2); // вперед
            Point<double> vb1 = pnorm(*pb3-*pb2); // вперед
            Point<double> va2 = pnorm((ab.x!=0)? *pa2-*pa3 : *pa1-*pa2); // назад
            Point<double> vb2 = pnorm((ab.y!=0)? *pb2-*pb3 : *pb1-*pb2); // назад
            // если отрезки va1-va2 и vb1-vb2 не пересекаются, то наша точка - касание
            find_cross(va1,va2,vb1,vb2);
	    crossings.push_back(crossing_t(pa2, pb2, ab.x*plength(*pa3-*pa2), ab.y*plength(*pb3-*pb2))); 
          } catch(int i){}
          pb1=pb2, pb2=pb3, pb3++;
        }
        pa1=pa2, pa2=pa3, pa3++;
      }

      //Добавим к линии точки пересечения. Заменим итераторы в crossings.
      // Для каждого пересечения сделаем пару 
      for (typename std::list<crossing_t>::iterator c=crossings.begin(); c!=crossings.end(); c++){
        typename Line<T>::iterator i;

        i = c->i1;
        while (i!=l.end()){ 
	  i++;
	  double ln = pdist(*i, *c->i1);
          if (ln < c->a1) {c->a1-=ln; c->i1 = i;} else break;
        }

        if (c->a1!=0){
          Point<T> p = *(c->i1);
          c->i1 = l.insert(i, Point<T>((T)(p.x + (i->x-p.x) * c->a1/plength(p - *i)),
                                       (T)(p.y + (i->y-p.y) * c->a1/plength(p - *i))));
	  c->a1 = 0;
        }

        i = c->i2; 
        while (i!=l.end()){ 
	  i++;
	  double ln = pdist(*i, *c->i2);
          if (ln<c->a2) {c->a2-=ln; c->i2 = i;} else break;
        }

        if (c->a2!=0){
          Point<T> p = *(c->i2);
          c->i2 = l.insert(i, Point<T>((T)(p.x + (i->x-p.x) * c->a2/plength(p - *i)),
                                       (T)(p.y + (i->y-p.y) * c->a2/plength(p - *i))));
	  c->a2 = 0;
        }
        crossings.insert(c, c->swap());
      }

      // собираем новую линюю из кусочков старой
      // если пересечений нет - то и все...
      if (crossings.size()==0) this->std::list<Line<T> >::push_back(l);
      // иначе...
      while (crossings.size()>0){
	Line<T> newline;
	// берем пересечение
        crossing_t cr = *crossings.begin();
        crossings.pop_front();

	// мы выйдем из cr.i2 и дойдем до cr.i1
        newline.push_back(*(cr.i2));
        while (1){
          cr.i2++;
          if (cr.i2 == l.end()) cr.i2 = l.begin();
          newline.push_back(*(cr.i2));
          if (cr.i1==cr.i2) break;
          // дошли ли мы до нового пересечения?
          for (typename std::list<crossing_t>::iterator f=crossings.begin(); f!=crossings.end(); f++){
            if (f->i1 == cr.i2){
              cr.i2 = f->i2; // перейдем на другую ветку
              crossings.erase(f);
              break;
            }
          }
        }
        this->std::list<Line<T> >::push_back(newline);
      }
    }

    void clear(){this->clear();}
};


// Принадлежит ли точка многоугольнику?
template <typename T> 
bool point_in_poly(const Polygon<T> & poly, const Point<T> & pt){
  int n = 0;
  for (typename Polygon<T>::const_iterator cnt = poly.begin(); cnt!=poly.end(); cnt++){
    int a = 0;
    typename Line<T>::const_iterator p1,p2;
    for (p1 = cnt.begin(), p2=p1+1; p2!=cnt.end(); p1++,p2++){
      a += acos(pscal(pnorm(pt-*p1),pnorm(pt-*p2)));
    }
std::cerr << "point_in_poly a = " << a << "\n";
    if (fabs(a)>M_PI) n++;
  }
  return n%2;
}


#endif /* POLY_H */
