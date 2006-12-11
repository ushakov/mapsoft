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
Rect<T> rect_intersect (Rect<T> const & R1, Rect<T> const & R2);

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

// при работе с целыми координатами иногда приятно считать,
// что rect.BRC не входит в прямоугольник
// (кстати, исходя из этого сделана и проверка на вхождение точки в прямоугольник)
// В этом случае, при делении на целое число надо пользоваться
// такой функцией:
Rect<int> rect_intdiv(const Rect<int> & r, int i);

// два прямоугольника задают преобразование.
// функция соответствующим образом сдвигает и растягивает третий прямоугольник
void transform_rect(const Rect<int> & src, const Rect<int> & dst, Rect<int> & r);
// Функция, нужная для загрузчика картинок.
// Правильное подрезание краев, выходящих за пределы картинки
void clip_rects_for_image_loader(
    const Rect<int> & src_img, Rect<int> & src,
    const Rect<int> & dst_img, Rect<int> & dst);

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
