#ifndef LINE_RECTCROP_H
#define LINE_RECTCROP_H

#include "line.h"
#include "rect.h"


/** ��������� ����� ��� �������������� line �� �������� �������������� cutter.
 ��� �������������� (closed=true) ���� ��-������� �������������� ������� ��
 ������ ����� �� ��������� */

template <typename T>
bool rect_crop(const Rect<T> & cutter, Line<T> & line, bool closed){

  T xl=cutter.x;
  T xh=cutter.x+cutter.w;
  T yl=cutter.y;
  T yh=cutter.y+cutter.h;

  if (line.size()<3) closed=false;
  Line<T> e; // for stable end iterator

  bool res=false;

  // ��� ������ ������� ��������������
  for (int i=0; i<4; i++){

    // ���� �� �����.
    // ���� ����� ������� �� �����, �� ������� ����� �������
    // �������������� - �������� �� �� ����� �����������
    // �������� � ��� ������ � ���� ������ (���� ��� ����)
    // � ��������� �� �� ������������ ��� ��������� ����� (skip=true).

    bool skip=false;
    typename Line<T>::iterator p=line.begin();
    while (p!=line.end()){
      // ���������� � ��������� �����
      // ���� �� ��� - ��� ��������������� � line.end()
      typename Line<T>::iterator pp = p, np = p;

      if (skip) pp=line.end();
      else {
        if (p==line.begin()){
          pp = line.end();
          if (closed) pp--;
        }
        else pp--;
      }

      np++;
      if ((np==line.end()) && closed) np=line.begin();

      // stable iterators
      if (pp==line.end()) pp=e.end();
      if (np==line.end()) np=e.end();

      // ��� ������� ������:
      if ((i==0) && (p->x>xh)){
        if ((pp!=e.end()) && (pp->x < xh)){
           p = line.insert(p, Point<T>(xh, p->y -
            ((p->y - pp->y)*(p->x - xh))/(p->x - pp->x) ));
           p++;
        }
        if ((np!=e.end()) && (np->x < xh)){
           p = line.insert(p, Point<T>(xh, p->y -
            ((p->y - np->y)*(p->x - xh))/(p->x - np->x) ));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==1) && (p->x<xl)){
        if ((pp!=e.end()) && (pp->x > xl)){
           p = line.insert(p, Point<T>(xl, p->y -
            ((p->y - pp->y)*(p->x - xl))/(p->x - pp->x) ));
           p++;
        }
        if ((np!=e.end()) && (np->x > xl)){
           p = line.insert(p, Point<T>(xl, p->y -
            ((p->y - np->y)*(p->x - xl))/(p->x - np->x) ));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==2) && (p->y>yh)){
        if ((pp!=e.end()) && (pp->y < yh)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - pp->x)*(p->y - yh))/(p->y - pp->y), yh));
           p++;
        }
        if ((np!=e.end()) && (np->y < yh)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - np->x)*(p->y - yh))/(p->y - np->y), yh));
           p++;
        }
        p=line.erase(p);
        skip=true;
        res=true;
        continue;
      }

      if ((i==3) && (p->y<yl)){
        if ((pp!=e.end()) && (pp->y > yl)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - pp->x)*(p->y - yl))/(p->y - pp->y), yl));
           p++;
        }
        if ((np!=e.end()) && (np->y > yl)){
           p = line.insert(p, Point<T>(p->x -
            ((p->x - np->x)*(p->y - yl))/(p->y - np->y), yl));
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

#endif

