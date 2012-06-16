#ifndef LINE_RECTCROP_H
#define LINE_RECTCROP_H

#include "line.h"
#include "rect.h"

///\addtogroup lib2d
///@{
///\defgroup line_rectcrop
///@{

/**
\brief Crop a line by a rectangle; line connectivity is kept but extra segments can appear on crop bounds.
Обрезание линии или многоугольника line по границам прямоугольника cutter.
Для многоугольника (closed=true) чуть по-другому обрабатывается отрезок от
первой точки до последней.
При обрезке не увеличивается количество объектов, зато могут возникать лишние
сегменты на границе прямоугольника (которые можно потом удалить с помощью
rect_split_cropped()).
*/

template <typename T>
bool rect_crop(const Rect<T> & cutter, Line<T> & line,
               bool closed=false){

  T xl=cutter.x;
  T xh=cutter.x+cutter.w;
  T yl=cutter.y;
  T yh=cutter.y+cutter.h;

  if (line.size()<3) closed=false;

  bool res=false;

  // для каждой стороны прямоугольника
  for (int i=0; i<4; i++){

    // Идем по линии.
    // Если точка выходит за линию, на которой лежит сторона
    // прямоугольника - заменяем ее на точки пересечения
    // соседних с ней сторон с этой линией (если они есть)
    // и исключаем ее из рассмотрения для следующей точки (skip=true).

    bool skip=false;
    typename Line<T>::iterator p=line.begin();
    while (p!=line.end()){
      // Предыдущая и следующая точки
      // Если их нет - они устанавливаются в line.end()
      typename Line<T>::iterator ppi = p, npi = p;

      if (skip) ppi=line.end();
      else {
        if (p==line.begin()){
          ppi = line.end();
          if (closed) ppi--;
        }
        else ppi--;
      }

      npi++;
      if ((npi==line.end()) && closed)
        npi=line.begin();

      bool pp_e = (ppi==line.end());
      bool np_e = (npi==line.end());
      Point<T> pp,np;
      if (ppi!=line.end()) pp=*ppi;
      if (npi!=line.end()) np=*npi;

      // для четырех сторон:
      if ((i==0) && (p->x>xh)){
        if (!pp_e && (pp.x < xh)){
           p = line.insert(p, Point<T>(xh, p->y -
            ((p->y - pp.y)*(p->x - xh))/(p->x - pp.x) ));
           p++;
        }
        if (!np_e && (np.x < xh)){
           p = line.insert(p, Point<T>(xh, p->y -
            ((p->y - np.y)*(p->x - xh))/(p->x - np.x) ));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==1) && (p->x<xl)){
        if (!pp_e && (pp.x > xl)){
           p = line.insert(p, Point<T>(xl, p->y -
            ((p->y - pp.y)*(p->x - xl))/(p->x - pp.x) ));
           p++;
        }
        if (!np_e && (np.x > xl)){
           p = line.insert(p, Point<T>(xl, p->y -
            ((p->y - np.y)*(p->x - xl))/(p->x - np.x) ));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==2) && (p->y>yh)){
        if (!pp_e && (pp.y < yh)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - pp.x)*(p->y - yh))/(p->y - pp.y), yh));
           p++;
        }
        if (!np_e && (np.y < yh)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - np.x)*(p->y - yh))/(p->y - np.y), yh));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==3) && (p->y<yl)){
        if (!pp_e && (pp.y > yl)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - pp.x)*(p->y - yl))/(p->y - pp.y), yl));
           p++;
        }
        if (!np_e && (np.y > yl)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - np.x)*(p->y - yl))/(p->y - np.y), yl));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }
      skip=false;
      p++;
    }
  }
  return res;
}

/**
  Разбить результат результат работы rect_crop() на отдельные линии
  с длиной больше 1, не лежащие на сторонах прямоугольника cutter.
*/
template <typename T>
MultiLine<T> rect_split_cropped(const Rect<T> & cutter, const Line<T> & cropped,
                                bool closed=false){
  MultiLine<T> ret;
  Line<T> rl;

  // Важные промежуточные переменные! Если сравнивать непосредственно
  // с cutter.x+cutter.w, то получится неточно и кое-что может не
  // сработать!
  T xl=cutter.x;
  T xh=cutter.x+cutter.w;
  T yl=cutter.y;
  T yh=cutter.y+cutter.h;

  // отдельные точки сохраняем
  if (cropped.size()==1){
    ret.push_back(cropped);
    return ret;
  }

  if (!closed){ // lines
    // Для линий просто выкидываю все сегменты, лежащие на границе
    // и отдельные точки-обрезки.
    typename Line<T>::const_iterator p, n;
    for (p=cropped.begin(); p!=cropped.end(); p++){
      n = p+1;
      rl.push_back(*p);

      // сегмент лежит на границе - нам такой не нужен!
      if ((n==cropped.end()) ||
          ((p->x == n->x) && (n->x == xl)) ||
          ((p->x == n->x) && (n->x == xh)) ||
          ((p->y == n->y) && (n->y == yl)) ||
          ((p->y == n->y) && (n->y == yh)) ){
        // нас не интересуют обрезки из одной точки:
        if (rl.size()>1) ret.push_back(rl);
        rl.clear();
      }
    }
  }
  else { //polygons
  // для многоугольников ищу пару сегментов, таких что
  // - оба лежат на одной границе
  // - один сегмент полностью лежит на другом
  // - сегменты имеют противоположное направление
  // по этой паре делаю разрез, прогоняю все заново.
    ret.push_back(cropped);
    bool repeat;
    do{
      repeat=false;
      typename MultiLine<T>::iterator l;
      for (l=ret.begin(); !repeat && (l!=ret.end()); l++){

        typename Line<T>::iterator p1a,p1b,p2a,p2b;
        for (p1a = l->begin();!repeat && (p1a!=l->end()) ; p1a++){
          p1b=p1a+1;
          if (p1b==l->end()) break;

          for (p2a = p1b; !repeat && (p2a!=l->end()); p2a++){
            p2b=p2a+1;
            if (p2b==l->end()) p2b=l->begin();

            // оба сегмента должны лежать на границе
            // и в правильном порядке - один внутри другого и навстречу друг другу.
            // Наверное, все можно в один if собрать...
            if (( (((p1a->x == xl) && (p1b->x == xl) && (p2a->x == xl) && (p2b->x == xl)) ||
                   ((p1a->x == xh) && (p1b->x == xh) && (p2a->x == xh) && (p2b->x == xh)) ) &&
                  (((p1a->y <= p2b->y) && (p2b->y <= p2a->y) && (p2a->y <= p1b->y)) ||
                   ((p2a->y <= p1b->y) && (p1b->y <= p1a->y) && (p1a->y <= p2b->y)) ||
                   ((p1a->y >= p2b->y) && (p2b->y >= p2a->y) && (p2a->y >= p1b->y)) ||
                   ((p2a->y >= p1b->y) && (p1b->y >= p1a->y) && (p1a->y >= p2b->y)) )) ||
                ( (((p1a->y == yl) && (p1b->y == yl) && (p2a->y == yl) && (p2b->y == yl)) ||
                   ((p1a->y == yh) && (p1b->y == yh) && (p2a->y == yh) && (p2b->y == yh)) ) &&
                  (((p1a->x <= p2b->x) && (p2b->x <= p2a->x) && (p2a->x <= p1b->x)) ||
                   ((p2a->x <= p1b->x) && (p1b->x <= p1a->x) && (p1a->x <= p2b->x)) ||
                   ((p1a->x >= p2b->x) && (p2b->x >= p2a->x) && (p2a->x >= p1b->x)) ||
                   ((p2a->x >= p1b->x) && (p1b->x >= p1a->x) && (p1a->x >= p2b->x)) ))) {
                Line<T> l1,l2;
                l1.insert(l1.end(), l->begin(), p1a+1);
                if (p2b!=l->begin()) l1.insert(l1.end(), p2b, l->end());
                l2.insert(l2.end(), p1b, p2a+1);
                l->swap(l1);
                ret.push_back(l2);
                repeat=true;
            }
          }
        }
      }
    } while (repeat);
  }
  return ret;
}

/**
  Произвести rect_crop(), а затем rect_split_cropped()
*/
template <typename T>
MultiLine<T> rect_crop_ml(const Rect<T> & cutter, const Line<T> & line,
                          bool closed=false){

  Line<T> cropped(line);
  rect_crop(cutter, cropped, closed);
  return rect_split_cropped(cutter, cropped, closed);
}


// fast and inaccurate test to find lines touching rectangle.
template <typename T>
bool rect_crop_test(const Rect<T> & cutter, const Line<T> & line, bool closed){

  T xl=cutter.x;
  T xh=cutter.x+cutter.w;
  T yl=cutter.y;
  T yh=cutter.y+cutter.h;

  if (line.size()<3) closed=false;
  Line<T> e; // for stable end iterator

  bool res=false;

  typename Line<T>::const_iterator p=line.begin();
  while (p!=line.end()){

    if (point_in_rect(*p, cutter)) return true;

    typename Line<T>::const_iterator np = p; np++;// next point
    if ((np==line.end()) && closed) np=line.begin();

    if (np==line.end()) break;
    if (!((p->x > xh) && (np->x > xh)) &&
        !((p->x < xl) && (np->x < xl)) &&
        !((p->y > yh) && (np->y > yh)) &&
        !((p->y < yl) && (np->y < yl))) return true;
    p++;
  }
  return false;
}

#endif
