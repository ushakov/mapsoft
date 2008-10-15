#ifndef RECT_H
#define RECT_H

#include <boost/operators.hpp>
#include "point.h"

template <typename T>
class Rect :
      public boost::multiplicative<Rect<T>,T>,
      public boost::additive<Rect<T>,Point<T> >,
      public boost::less_than_comparable<Rect<T> >,
      public boost::equality_comparable<Rect<T> >
{
public:

    T x,y,w,h;

    Rect (Point<T> p1, Point<T> p2)
	: x(std::min(p1.x, p2.x)), y(std::min(p1.y, p2.y)),
	  w(std::max(p1.x,p2.x) - std::min(p1.x,p2.x)),
	  h(std::max(p1.y,p2.y) - std::min(p1.y,p2.y)) {
    }

    Rect (T _x, T _y, T _w=0, T _h=0)
	: x(_x), y(_y), w(_w), h(_h)
    {
	if ((w<0)||(h<0)) std::cerr 
          << "Rect warning: bad geometry: "
          << w << "x" << h << "+" << x << "+" << y << "\n";
    }

    Rect () : x(0), y(0), w(0), h(0)
    { }


    bool empty() const { return w <= 0 || h <= 0; }

    Point<T> TLC() const { return  Point<T>(x,y);}
    Point<T> TRC() const { return  Point<T>(x+w,y);}
    Point<T> BRC() const { return  Point<T>(x+w,y+h);}
    Point<T> BLC() const { return  Point<T>(x,y+h);}

    Rect<T> & operator/= (T k)
    {
        x /= k;
        y /= k;
        w /= k;
        h /= k;
        return *this;
    }

    Rect<T> & operator*= (T k)
    {
        x *= k;
        y *= k;
        w *= k;
        h *= k;
        return *this;
    }

    Rect<T> & operator-= (Point<T> p){
        x -= p.x;
        y -= p.y;
        return *this;
    }

    Rect<T> & operator+= (Point<T> p){
        x += p.x;
        y += p.y;
        return *this;
    }

  bool operator< (const Rect<T> & r) const
  {
        if (TLC()!=r.TLC()) return (TLC()<r.TLC());
        return (BRC()<r.BRC());
  }

  bool operator== (const Rect<T> & r) const
  {
        return (x==r.x)&&(y==r.y)&&(w==r.w)&&(h==r.h);
  }


};

template <typename T>
std::ostream & operator<< (std::ostream & s, const Rect<T> & r){
  // std::showpos don't add + to zeros :(
  s << r.w << "x" << r.h << ((r.x>=0)? "+":"") << r.x << ((r.y>=0)? "+":"") << r.y;
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Rect<T> & r){
  char sep;

  s >> r.w >> sep >> r.h;
  if (sep!='x') return s;

  s >> sep >> r.x;
  if (sep=='-') r.x=-r.x;
  else if (sep!='+') return s;

  s >> sep >> r.y;
  if (sep=='-') r.y=-r.y;
  else if (sep!='+') return s;

  return s;
}


template <typename T>
Rect<T> rect_pump (Rect<T> const & R, T val) {
    return Rect<T> (R.x-val, R.y-val, R.w+2*val, R.h+2*val);
}

template <typename T>
Rect<T> rect_pump (Rect<T> & R, Point<T> p) {
    Rect<T> ret = R;
    if      (R.x>p.x)     {ret.x=p.x; ret.w+=R.x-p.x;}
    else if (R.x+R.w<p.x) {           ret.w=p.x-R.x;}
    if      (R.y>p.y)     {ret.y=p.y; ret.h+=R.y-p.y;}
    else if (R.y+R.h<p.y) {           ret.h=p.y-R.y;}
    return ret;
}

template <typename T>
Rect<T> rect_pump (Rect<T> const & R, Rect<T> bounds) {
    return Rect<T> (R.x-bounds.BRC().x, R.y-bounds.BRC().y, R.w+bounds.w, R.h+bounds.h);
}

template <typename T>
Rect<T> rect_intersect (Rect<T> const & R1, Rect<T> const & R2){
    T x1 =  std::max (R1.x, R2.x);
    T y1 =  std::max (R1.y, R2.y);
    T x2 =  std::min (R1.x+R1.w, R2.x+R2.w);
    T y2 =  std::min (R1.y+R1.h, R2.y+R2.h);
    T w = x2-x1;
    T h = y2-y1;
    if (w<0) w=0;
    if (h<0) h=0;
    return Rect<T>(x1,y1,w,h);
}

template <typename T>
Rect<T> rect_bounding_box (Rect<T> const & R1, Rect<T> const & R2){
    if (R1.empty()) return R2;
    if (R2.empty()) return R1;
    T x1 =  std::min (R1.x, R2.x);
    T y1 =  std::min (R1.y, R2.y);
    T x2 =  std::max (R1.x+R1.w, R2.x+R2.w);
    T y2 =  std::max (R1.y+R1.h, R2.y+R2.h);
    T w = x2-x1;
    T h = y2-y1;
    return Rect<T>(x1,y1,w,h);
}

// точка не всегда будет входит в прямоугольник!!! 
template <typename T>
void clip_point_to_rect (Point<T> & p, const Rect<T> & r){
    p.x = std::max (r.x, p.x);
    p.x = std::min (r.x+r.w, p.x);
    p.y = std::max (r.y, p.y);
    p.y = std::min (r.y+r.h, p.y);
// точка не всегда будет входит в прямоугольник!!! Исправить?
}

template <typename T>
void clip_rect_to_rect (Rect<T> & r1, const Rect<T> & r2){
    r1 = rect_intersect(r1,r2);
}

template <typename T>
bool point_in_rect (const Point<T> & p, const Rect<T> & r){
    return (r.x <= p.x) && (r.x+r.w > p.x) &&
           (r.y <= p.y) && (r.y+r.h > p.y);
}

/*
// при работе с целыми координатами иногда приятно считать,
// что rect.BRC не входит в прямоугольник
// (кстати, исходя из этого сделана и проверка на вхождение точки в прямоугольник)
*/

// диапазон плиток, накрывающих данный прямоугольник
Rect<int> tiles_on_rect(const Rect<int> & r, int tsize);
// диапазон плиток, лежащих внутри данного прямоугольника
Rect<int> tiles_in_rect(const Rect<int> & r, int tsize);


// два прямоугольника задают преобразование.
// функция соответствующим образом сдвигает и растягивает третий прямоугольник
void transform_rect(const Rect<int> & src, const Rect<int> & dst, Rect<int> & r);
// Функция, нужная для загрузчика картинок.
// Правильное подрезание краев, выходящих за пределы картинки
void clip_rects_for_image_loader(
    const Rect<int> & src_img, Rect<int> & src,
    const Rect<int> & dst_img, Rect<int> & dst);

#endif /* RECT_H */
