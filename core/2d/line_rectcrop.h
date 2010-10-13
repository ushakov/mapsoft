#ifndef LINE_RECTCROP_H
#define LINE_RECTCROP_H

#include "line.h"
#include "rect.h"


/** Обрезание линии или многоугольника line по границам прямоугольника cutter.
 Для многоугольника (closed=true) чуть по-другому обрабатывается отрезок от
 первой точки до последней.
 Одна из основных особенностей - при обрезке не увеличивается количество
 объектов.
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

// То же, но без добавления новых точек
// (для обрезки куска карты в надежде, что потом его можно будет вклеить обратно).
// Выглядит не слишком красиво, так как могут оставаться очень далекие от
// района обрезки точки (необходимые, чтоб обойти угол).
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

