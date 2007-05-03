#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <list>
#include <map>
#include <set>

#include "point.h"

// Линия -- список точек

template <typename T> 
struct Line : std::list<Point<T> > {
};

// Процедура поиска пересечения двух отрезков
// Если отрезки не пересекаются - возникает исключение!
// Отрезок включает первую точку, но не включает последнюю

template <typename T> 
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2, 
                    const Point<T> & p3, const Point<T> & p4){
  Point<T> p12 = p2-p1;
  Point<T> p43 = p3-p4;
  Point<T> p14 = p4-p1;
  double k1 = p12.x*p43.y - p12.y*p43.x;
  double k2 = p14.x*p43.y - p14.y*p43.x;
  if (k1==0) throw; // параллельные линии(?)
  double a = k2/k1;
  if ((a<0)||(a>=1)) throw 0; // пересечение - за пределами отрезка
  return Point<T>((T)(p1.x+a*p12.x), (T)(p1.y+a*p12.y));
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
      typedef std::map<typename Line<T>::iterator, typename Line<T>::iterator> crossings_t;
      crossings_t crossings;

      // Разыщем все пересечения:
      typename Line<T>::iterator p1,p2,p3,p4;
      for (p1 = l.begin(); p1!=l.end(); p1++){
        p2=p1; p2++; if (p2==l.end()) continue;
        for (p3 = p2; p3!=l.end(); p3++){
          p4=p3; p4++; if (p4==l.end()) continue;
          try {
            Point<T> p = find_cross(*p1,*p2,*p3,*p4);
	    typename Line<T>::iterator i1 = (p==*p1)? p1 : l.insert(p1,p);
	    typename Line<T>::iterator i2 = (p==*p3)? p3 : l.insert(p3,p);
            crossings[i1] = i2;
            crossings[i2] = i1;
          } catch(int i){}
        }
      }
      
      while (crossings.size()>0){
	// собираем новую линюю из кусочков старой
	Line<T> newline;

        std::pair<typename Line<T>::iterator, typename Line<T>::iterator> 
          cr = *crossings.begin();
          crossings.erase(crossings.begin());
        // пока не придем к тому же перекрестку
        while (cr.first!=cr.second){
          // дошли ли мы до нового пересечения?
          typename crossings_t::iterator f = crossings.find(cr.first);
          if (f!=crossings.end()){
            cr.first = f->second; // перейдем на другую ветку
            crossings.erase(f);
          }
          // дошли ли мы до начала линии?
	  if (cr.first == l.end()) cr.first=l.begin();
          newline.push_back(*(cr.first));
        }
        this->push_back(newline);
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
