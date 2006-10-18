#ifndef RECT_H
#define RECT_H

#include <boost/operators.hpp>
#include "point.h"

template <typename T>
class Rect : 
      public boost::multiplicative<Rect<T>,T>,
      public boost::additive<Rect<T>,Point<T> >
{
public:

    T x,y,w,h;

    Rect (Point<T> p1, Point<T> p2)
	: x(p1.x), y(p1.y), w(p2.x-p1.x),h(p2.y-p1.y) { 
	if (w<0) std::cerr << "Rect warning: w<0\n";
	if (h<0) std::cerr << "Rect warning: h<0\n";
    }

    Rect (T _x, T _y, T _w=0, T _h=0)
	: x(_x), y(_y), w(_w), h(_h)
    { 
	if (w<0) std::cerr << "Rect warning: w<0\n";
	if (h<0) std::cerr << "Rect warning: h<0\n";
    }

    Rect () : x(0), y(0), w(0), h(0)
    { }


    bool empty() const { return w <= 0 || h <= 0; }

    Point<T> TLC() const { return  Point<T>(x,y);}
    Point<T> BRC() const { return  Point<T>(x+w,y+h);}

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

};

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
    T x1 =  std::min (R1.x, R2.x);
    T y1 =  std::min (R1.y, R2.y);
    T x2 =  std::max (R1.x+R1.w, R2.x+R2.w);
    T y2 =  std::max (R1.y+R1.h, R2.y+R2.h);
    T w = x2-x1;
    T h = y2-y1;
    return Rect<T>(x1,y1,w,h);
}


template <typename T>
void clip_point_to_rect (Point<T> & p, const Rect<T> & r){
    p.x = std::max (r.x, p.x);
    p.x = std::min (r.x+r.w, p.x);
    p.y = std::max (r.y, p.y);
    p.y = std::min (r.y+r.h, p.y);
}

template <typename T>
void clip_rect_to_rect (Rect<T> & r1, const Rect<T> & r2){
//    std::cerr << r1 << " clip to " << r2 << "  ";
    r1 = rect_intersect(r1,r2);
//    std::cerr << r1 << "\n";
}

template <typename T>
bool point_in_rect (const Point<T> & p, const Rect<T> & r){
    return (r.x <= p.x) && (r.x+r.w > p.x) &&
           (r.y <= p.y) && (r.y+r.h > p.y);
}

// ��� ������ � ������ ������������ ������ ������� �������,
// ��� rect.BRC �� ������ � �������������
// (������, ������ �� ����� ������� � �������� �� ��������� ����� � �������������)
// � ���� ������, ��� ������� �� ����� ����� ���� ������������
// ����� ��������:
Rect<int> rect_intdiv(const Rect<int> & r, int i){

      if (i==1) return r;

      int x1 = r.x; 
      int y1 = r.y; 
      int x2 = r.x+r.w-1; 
      int y2 = r.y+r.h-1; 

      x1 = x1<0 ? x1/i - 1  : x1/i; 
      y1 = y1<0 ? y1/i - 1  : y1/i; 
      x2 = x2<0 ? x2/i - 1  : x2/i; 
      y2 = y2<0 ? y2/i - 1  : y2/i; 

      return Rect<int>(x1,y1,x2-x1+1,y2-y1+1);
}

// ��� �������������� ������ ��������������.
// ������� ��������������� ������� �������� � ����������� ������ �������������
template <typename T>
void transform_rect(
    const Rect<T> & src,
    const Rect<T> & dst,
          Rect<T> & r){
    r.x = dst.x + ((r.x-src.x)*dst.w)/src.w;
    r.y = dst.y + ((r.y-src.y)*dst.h)/src.h;
    r.w = (r.w*dst.w)/src.w;
    r.h = (r.h*dst.h)/src.h;
}

// �������, ������ ��� ���������� ��������.
// ���������� ���������� �����, ��������� �� ������� ��������
template <typename T>
void clip_rects_for_image_loader(
    const Rect<T> & src_img,
          Rect<T> & src,
    const Rect<T> & dst_img,
          Rect<T> & dst){
   Rect<T> src_img_tr = src_img; transform_rect(src,dst,src_img_tr);
   Rect<T> dst_img_tr = dst_img; transform_rect(dst,src,dst_img_tr);

   clip_rect_to_rect(src, src_img);
   clip_rect_to_rect(dst, src_img_tr);
   clip_rect_to_rect(src, dst_img_tr);
   clip_rect_to_rect(dst, dst_img);
}


template <typename T>
std::ostream & operator<< (std::ostream & s, const Rect<T> & r)
{
  s << "Rect(" 
    << r.w << "x" << r.h 
    << "+" << r.x 
    << "+" << r.y 
    << ")";
  return s;
}

#endif /* RECT_H */
