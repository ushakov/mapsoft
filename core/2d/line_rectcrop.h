#ifndef LINE_RECTCROP_H
#define LINE_RECTCROP_H

#include "line.h"
#include "rect.h"

///\addtogroup lib2d
///@{
///\defgroup rectcrop
///@{

/**
\brief Crop a line by a rectangle; line connectivity is kept but extra segments cat appear on crop bounds.
  Обрезание линии или многоугольника line по границам прямоугольника cutter.
  Для многоугольника (closed=true) чуть по-другому обрабатывается отрезок от
  первой точки до последней.
  При обрезке не увеличивается количество объектов, зато могут возникать лишние
  сегменты на границе прямоугольника.
*/

template <typename T>
bool rect_crop(const Rect<T> & cutter, Line<T> & line, bool closed){

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
  То же, но без добавления новых точек
  (для обрезки куска карты в надежде, что потом его можно будет вклеить обратно).
  Выглядит не слишком красиво, так как могут оставаться очень далекие от
  района обрезки точки (необходимые, чтоб обойти угол).
*/
template <typename T>
bool rect_crop_noadd(const Rect<T> & cutter, Line<T> & line, bool closed){

  T xl=cutter.x;
  T xh=cutter.x+cutter.w;
  T yl=cutter.y;
  T yh=cutter.y+cutter.h;

  if (line.size()<3) closed=false;

  bool res=false;

  // Идем по линии.
  // Если точка и ее соседи выходят за одну из сторон
  // прямоугольника - удаляем точку

  typename Line<T>::iterator p=line.begin();
  while (p!=line.end()){

    // Предыдущая и следующая точки
    // Если их нет - они устанавливаются в line.end()
    typename Line<T>::iterator ppi = p, npi = p;

    if (p==line.begin()){
      ppi = line.end();
      if (closed) ppi--;
    }
    else ppi--;

    npi++;
    if ((npi==line.end()) && closed)
      npi=line.begin();

    bool pp_e = (ppi==line.end());
    bool np_e = (npi==line.end());
    Point<T> pp,np;
    if (ppi!=line.end()) pp=*ppi;
    if (npi!=line.end()) np=*npi;

    if ((p->x > xh) &&
        (pp_e || (pp.x > xh)) &&
        (np_e || (np.x > xh))){
      p=line.erase(p);
      res=true;
      continue;
    }
    if ((p->x < xl) &&
        (pp_e || (pp.x < xl)) &&
        (np_e || (np.x < xl))){
      p=line.erase(p);
      res=true;
      continue;
    }
    if ((p->y > yh) &&
        (pp_e || (pp.y > yh)) &&
        (np_e || (np.y > yh))){
      p=line.erase(p);
      res=true;
      continue;
    }
    if ((p->y < yl) &&
        (pp_e || (pp.y < yl)) &&
        (np_e || (np.y < yl))){
      p=line.erase(p);
      res=true;
      continue;
    }
    p++;
  }
  return res;
}

/**
  Разбить результат результат работы rect_crop() на отдельные линии
  с длиной больше 1, не лежащие на сторонах прямоугольника cutter.
*/
template <typename T>
MultiLine<T> rect_split_cropped(const Rect<T> & cutter, const Line<T> & cropped){
  MultiLine<T> ret;
  Line<T> rl;

  typename Line<T>::const_iterator p, n;
  for (p=cropped.begin(); p!=cropped.end(); p++){
    n = p+1;

    if (n!=cropped.end()){
      // сегмент лежит на границе - нам такой не нужен!
      if (((p->x == n->x) && (n->x == cutter.x)) ||
          ((p->x == n->x) && (n->x == cutter.x+cutter.w)) ||
          ((p->y == n->y) && (n->y == cutter.y)) ||
          ((p->y == n->y) && (n->y == cutter.y+cutter.h))){
        rl.push_back(*p);
        if (rl.size()>1) ret.push_back(rl);
        rl.clear();
        continue;
      }
    }
    rl.push_back(*p);
  }
  if (rl.size()>1) ret.push_back(rl);
  return ret;
}

/**
  Произвести rect_crop(), а затем rect_split_cropped()
*/
template <typename T>
MultiLine<T> rect_crop_ml(const Rect<T> & cutter, const Line<T> & line){

  Line<T> cropped(line);
  rect_crop(cutter, cropped, false);
  return rect_split_cropped(cutter, cropped);
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
