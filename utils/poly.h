#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "point.h"

// Линия -- список точек

template <typename T> 
struct Line : std::list<Point<T> > {
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
  Point<double> ab = ind_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // пересечение - за пределами отрезка
  if ((ab.y<0)||(ab.y>=1)) throw 0; // пересечение - за пределами отрезка
  return Point<T>((T)(p1.x+ab.x*(p2.x-p1.x)), (T)(p1.y+ab.x*(p2.y-p1.y)));
}


// Обобщенный многоугольник:
// Есть множество замкнутых контуров. 
// "положительный" контур - обходится по часовой стрелке,
// "отрицательный" - против. 

// Контур с самопересечениями разбивается на несколько 
// контуров разных знаков. (Подумать еще про это! Видимо, надо
// где-то разбивать специально?)

// Точка плоскости принадлежит многоугольнику, 
// если число охватывающих ее положительных контуров
// плюс число не охватывающих ее отрицательных контуров нечетно.

// Другими словами: положительный контур меняет состояние всех точек
// внутри себя, отрицательный - вне себя.
// ("восьмерка" меняет состояние точек вне себя :))
// 

template<typename X, typename Y>
bool sort_pairs(const std::pair<X,Y> & p1, const std::pair<X,Y> & p2)
     {return p1.first < p2.first;}


template <typename T> 
struct Polygon : std::list<Line<T> > {



    void push_back(Line<T> l){
      // Добавление контура.
      // Если в контуре меньше трех точек - нам не нужен такой
      if (l.size()<3) return;
      // Последняя точка должна иметь те же координаты, что и первая.
      if (*l.begin() != *l.rbegin()) l.push_back(*l.begin());

      // Контур с самопересечениями нужно разбить на несколько.

      // Будем так хранить пересечения
      typedef std::pair<typename Line<T>::iterator, typename Line<T>::iterator> crossing_t;
      typedef std::list<crossing_t> crossings_t;
      crossings_t crossings;

      // Разыщем все пересечения.
      // Для каждого звена ищем пересечения cо всеми следующими звеньями.
      // Добавляем в них точки пересечения сразу, а в исходное звено - 
      // потом, упорядочив нужным образом.

      typename Line<T>::iterator p1,p2=l.begin();

      while (p2!=l.end()){
	p1=p2; p2++; if (p2==l.end()) break;

        typedef std::pair<double, typename Line<T>::iterator> my_as_pt;
        std::vector<my_as_pt> as;

        typename Line<T>::iterator p3,p4=p2; 
        p4++; //(два соседних звена не пересекаются :))
        while (p4!=l.end()){
	  p3=p4; p4++; if (p4==l.end()) break;

	  try {
            Point<double> ab = find_cross_ab(*p1,*p2,*p3,*p4);
            if ((ab.x<0)||(ab.x>=1)||(ab.y<0)||(ab.y>=1)) continue;
            // Добавим точку на вторую линию (если это не ее начальная точка)
            // добавим запись в as
            if (ab.y==0){as.push_back(my_as_pt(ab.x,p3));}
            else {
              as.push_back(my_as_pt(ab.x, 
                l.insert(p4,
                  Point<T>((T)(p3->x + ab.y*(p4->x - p3->x)), 
                           (T)(p3->y + ab.y*(p4->y - p3->y))))
                )
              );
            }
          } catch(int i){}
        }
	// Теперь отсортируем пересечения, добавим их на первую линию,
        // Добавим в crossings переходы с одной линии на другую и обратно:
        std::sort(as.begin(), as.end(), sort_pairs<double, typename Line<T>::iterator>);

        for (typename std::vector<my_as_pt>::const_iterator p=as.begin(); p!=as.end(); p++){
          typename Line<T>::iterator i1 = l.insert(p2, 
            Point<T>((T)(p1->x + p->first*(p2->x - p1->x)),
                     (T)(p1->y + p->first*(p2->y - p1->y)))
          );
	    // переход туда и обратно
          crossings.push_back(crossing_t(i1,p->second));
          crossings.push_back(crossing_t(p->second,i1));
        }
      }


      // собираем новую линюю из кусочков старой

      // если пересечений нет - то и все...
      if (crossings.size()==0) this->std::list<Line<T> >::push_back(l);

      while (crossings.size()>0){
	Line<T> newline;

	// берем переход
        crossing_t cr = *crossings.begin();
        crossings.pop_front();

	// мы выйдем из cr.second и дойдем до cr.first
        newline.push_back(*(cr.second));
        while (1){
          cr.second++;
          if (cr.second == l.end()) cr.second = l.begin();
          newline.push_back(*(cr.second));
          if (cr.first==cr.second) break;
          // дошли ли мы до нового пересечения?
          for (typename crossings_t::iterator f=crossings.begin(); f!=crossings.end(); f++){
            if (f->first == cr.second){
              cr.second = f->second; // перейдем на другую ветку
              crossings.erase(f);
              break;
            }
          }
        }
        this->std::list<Line<T> >::push_back(newline);
      }
    }
};


// Определение знака контура
// тут могут быть такие варианты:
// - знак интеграла xdy или ydx
// - выбрать точку с минимальной координатой и посмотреть
//   разницу азимутов на следующую и предыдущую точки
// (но с самопересечениями это будет работать некорректно!)
template <typename T> 
int polygon_sign(const Polygon<T> & p){

}
// Принадлежит ли точка многоугольнику
// - для каждого контура найти ближайшую точку, 

template <typename T> 
bool pt_in_polygon(const Polygon<T> & plg, const Point<T> & p){
}

// Для двух отрезков p1-p2 и p3-p4 найти точку пересечения


#endif /* POLY_H */
