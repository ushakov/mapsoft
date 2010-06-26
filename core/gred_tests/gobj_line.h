#ifndef GOBJ_LINE_H
#define GOBJ_LINE_H

#include "iface/gobj.h"
#include "lib2d/line.h"

/*****************/

void brez(iImage &img, int x1, int y1, int x2, int y2, int c){
  int dx=x2-x1, dy=y2-y1;
  int e;
  int sx=dx>0;
  int sy=dy>0;

  if (!sx) dx=-dx;
  if (!sy) dy=-dy;

  int s=dx>dy;

  img.safe_set(x1,y1,c);
  if (s){
    e = (dy<<1)-dx;
    while (x1!=x2){
      // если шаг по x не приведет к большому отклонению,
      // сделаем шаг по оси x
      if (e<0){
        sx?x1++:x1--; e+=dy<<1;
        img.safe_set(x1,y1,c);
      }
      // иначе - шаг по оси y
      else {
        sy?y1++:y1--; e-=dx<<1;
      }
    }
  }
  else {
    e = (dx<<1)-dy;
    while (y1!=y2){
      // если шаг по y не приведет к большому отклонению,
      // сделаем шаг по оси y
      if (e<0){
        sy?y1++:y1--; e+=dx<<1;
        img.safe_set(x1,y1,c);
      }
      // иначе - шаг по оси x
      else {
        sx?x1++:x1--; e-=dy<<1;
      }
    }
  }
}

/*****************/

bool seg_outside_box(const iPoint &p1, const iPoint &p2, const iRect &r){
  return
    ( (p1.x<r.x) && (p2.x<r.x) ) ||
    ( (p1.x>r.x+r.w) && (p2.x>r.x+r.w) ) ||
    ( (p1.y<r.y) && (p2.y<r.y) ) ||
    ( (p1.y>r.y+r.h) && (p2.y>r.y+r.h) );
}

/*****************/

class GObjLine: public GObj{
  int color, thickness;
  iLine line;
  iRect line_range;

public:
  GObjLine(const iLine & line_, const int color_, const int thickness_):
    line(line_), color(color_), thickness(thickness_), line_range(line.range()){
  }

  GObjLine & crop(const iRect & r){

  }

  int draw(iImage &img, const iPoint &origin){
    iLine::const_iterator i,j;
    iRect r(origin.x, origin.y, img.w, img.h);

    for (j=i=line.begin(), j++; j!=line.end(); i++,j++){
      if (seg_outside_box(*i,*j, r)) continue;
      iPoint p1=*i-origin, p2=*j-origin;
      brez(img, p1.x, p1.y, p2.x, p2.y, color);
    }
  }
  iRect range(){ return line_range; }
};

#endif
