#ifndef GOBJ_LINE_H
#define GOBJ_LINE_H

#include "iface/gobj.h"
#include "lib2d/line.h"
#include "utils/cache.h"

#define CACHE_SIZE 100

/*****************/

void bres_line(iImage &img, int x1, int y1, int x2, int y2, int w, int c){
  int dx=x2-x1, dy=y2-y1;
  int e;
  int sx=dx>0;
  int sy=dy>0;

  if (!sx) dx=-dx;
  if (!sy) dy=-dy;

  int s=dx>dy;

  int w1=w>>1;
  int w2=w>>1;
  if (w1+w2<w) w2++;
  int j;

  if (s){
    for (j=-w1;j<w2;j++) img.safe_set(x1,y1+j,c);
    e = (dy<<1)-dx;
    while (x1!=x2){
      // если шаг по x не приведет к большому отклонению,
      // сделаем шаг по оси x
      if (e<0){
        sx?x1++:x1--;
        e+=dy<<1;
        for (j=-w1;j<w2;j++) img.safe_set(x1,y1+j,c);
      }
      // иначе - шаг по оси y
      else {
        sy?y1++:y1--;
        e-=dx<<1;
      }
    }
  }
  else {
    for (j=-w1;j<w2;j++) img.safe_set(x1+j,y1,c);
    e = (dx<<1)-dy;
    while (y1!=y2){
      // если шаг по y не приведет к большому отклонению,
      // сделаем шаг по оси y
      if (e<0){
        sy?y1++:y1--; e+=dx<<1;
        for (j=-w1;j<w2;j++) img.safe_set(x1+j,y1,c);
      }
      // иначе - шаг по оси x
      else {
        sx?x1++:x1--; e-=dy<<1;
      }
    }
  }
}



/// Move this to lib2d?

/************************************/
// move p to the rect boundary
iPoint move_pt_to_rect(const iPoint &p1, const iPoint &p2, const iRect &r){
   if (p1==p2) return p1;
   int b;
   iPoint p=p1;
   b = r.x;
   if ((p1.x < b) && (p2.x >= b)){
     int y = p1.y + (p2.y - p1.y)*(b - p1.x) / (p2.x - p1.x);
     if ((y >= r.y) && (y < r.y+r.h)) p = iPoint(b,y);
   }
   b = r.x + r.w - 1;
   if ((p1.x > b) && (p2.x <= b)){
     int y = p2.y + (p1.y - p2.y)*(b - p2.x) / (p1.x - p2.x);
     if ((y >= r.y) && (y < r.y+r.h)) p = iPoint(b,y);
   }
   b = r.y;
   if ((p1.y < b) && (p2.y >= b)){
     int x = p1.x + (p2.x - p1.x)*(b - p1.y) / (p2.y - p1.y);
     if ((x >= r.x) && (x < r.x+r.w)) p = iPoint(x,b);
   }
   b = r.y + r.h - 1;
   if ((p1.y > b) && (p2.y <= b)){
     int x = p2.x + (p1.x - p2.x)*(b - p2.y) / (p1.y - p2.y);
     if ((x >= r.x) && (x < r.x+r.w)) p = iPoint(x,b);
   }
   return p;
}

iMultiLine
crop_and_scale_multiline(const iMultiLine & line, const iRect & cutter, const double k){
  iMultiLine ret;

  iMultiLine::const_iterator m;
  iLine::const_iterator i,j;
  iLine l;

  for (m=line.begin(); m!=line.end(); m++){
    for (i=m->begin(); i!=m->end(); i++){

      iPoint p0(dPoint(*i)*k);
      // if point inside rect then just put it into current line
      if (point_in_rect(p0,cutter)){
        if ((l.rbegin() == l.rend()) || (*l.rbegin() != p0))
          l.push_back(p0);
        continue;
      }

      // if point outside rect...

      // previous segment
      if (i!=m->begin()){
        j=i; j--;
        // move p to the rect boundary:
        iPoint p = move_pt_to_rect(p0, dPoint(*j)*k, cutter);
        // point was not moved if the whole segment outside rect
        if (p != p0) l.push_back(p);

        if (l.size()>0){
          ret.push_back(l);
          l.clear();
        }
      }

      // next segment
      j=i; j++;
      if (j!=m->end()){
        iPoint p = move_pt_to_rect(p0,dPoint(*j)*k, cutter);
        // point was not moved if the whole segment outside rect
        if (p != p0) l.push_back(p);
      }
    }
    if (l.size()>0){ ret.push_back(l); l.clear(); }
  }
  return ret;
}

/******************************/

class GObjLine: public GObj{
  int color, thickness, sc_thickness;
  double sc;
  iMultiLine line;
  iRect line_range;

  Cache<iRect, iMultiLine> cache;

public:
  GObjLine(const iMultiLine & line_, const int color_, const int thickness_):
       line(line_), color(color_),
       thickness(thickness_), sc_thickness(thickness_),
       sc(1.0), line_range(line.range()),
    cache(CACHE_SIZE){
  }

  void rescale(const double k){
    sc=k;
    line_range = dRect(line.range())*k;
    sc_thickness = thickness*sc;
    if (sc_thickness==0) sc_thickness++;
    cache.clear();
  }
  void refresh(){
    cache.clear();
  }

  void draw_mline(iImage &img, const iPoint &origin, const iMultiLine &l){
    iMultiLine::const_iterator m;
    iLine::const_iterator i,j;

    for (m=l.begin(); m!=l.end(); m++){
      for (j=i=m->begin(), j++; j!=m->end(); i++,j++){
        iPoint p1=*i-origin, p2=*j-origin;
        bres_line(img, p1.x, p1.y, p2.x, p2.y, sc_thickness, color);
      }
    }
  }

  int draw(iImage &img, const iPoint &origin){
    iRect r(origin.x, origin.y, img.w, img.h);

    if (!cache.contains(r)){
      cache.add(r,
         crop_and_scale_multiline(line, rect_pump(r,sc_thickness/2), sc));
    }

    iMultiLine & l = cache.get(r);
    draw_mline(img, origin, l);
    return (l.size() >0) ? GOBJ_FILL_PART : GOBJ_FILL_NONE;
  }

  iRect range() const{
    return line_range;
  }


};

#endif
