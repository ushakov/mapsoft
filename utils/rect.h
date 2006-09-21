#ifndef RECT_H
#define RECT_H

#include <boost/operators.hpp>
#include "point.h"

template <typename T>
class Rect     
    : public boost::multiplicative<Point<T>,T>
{
public:

    Point<T> TLC, BRC; // TLC -- минимум x и y

    Rect (Point<T> p1, Point<T> p2)
	: TLC(p1), BRC(p2)
    { 
	if (TLC.x>BRC.x) std::swap(TLC.x,BRC.x);
	if (TLC.y>BRC.y) std::swap(TLC.y,BRC.y);
    }

    Rect (int x1, int y1, int x2, int y2)
	: TLC(x1,y1), BRC(x2,y2)
    { 
	if (TLC.x>BRC.x) std::swap(TLC.x,BRC.x);
	if (TLC.y>BRC.y) std::swap(TLC.y,BRC.y);
    }

    Rect ()
	: TLC(0,0), BRC(0,0)
    { }


    bool empty() const
    {
	return TLC.x >= BRC.x || TLC.y >= BRC.y;
    }

    T width() const {
	return BRC.x-TLC.x;
    }

    T height() const {
	return BRC.y-TLC.y;
    }

    Rect<T> & operator/= (T k)
    {
        TLC /= k;
        BRC /= k;
        return *this;
    }

    Rect<T> & operator*= (T k)
    {
        TLC *= k;
        BRC *= k;
        return *this;
    }

};

template <typename T>
Rect<T> intersect (Rect<T> const & R1, Rect<T> const & R2){
    if (R1.empty()) return R1;
    if (R2.empty()) return R2;
    return Rect<T>(
      std::max (R1.TLC.x, R2.TLC.x), 
      std::max (R1.TLC.y, R2.TLC.y),
      std::min (R1.BRC.x, R2.BRC.x),
      std::min (R1.BRC.y, R2.BRC.y));
}

template <typename T>
Rect<T> bounding_box (Rect<T> const & R1, Rect<T> const & R2){
    if (R1.empty()) return R2;
    if (R2.empty()) return R1;
    return Rect<T>(	
      std::min (R1.TLC.x, R2.TLC.x),
      std::min (R1.TLC.y, R2.TLC.y),
      std::max (R1.BRC.x, R2.BRC.x),
      std::max (R1.BRC.y, R2.BRC.y));
}


template <typename T>
void clip_point_to_rect (Point<T> & p, const Rect<T> & r){
    p.x = std::max (r.TLC.x, p.x);
    p.x = std::min (r.BRC.x, p.x);
    p.y = std::max (r.TLC.y, p.y);
    p.y = std::min (r.BRC.y, p.y);
}

template <typename T>
void clip_rect_to_rect (Rect<T> & r1, const Rect<T> & r2){
    clip_point_to_rect(r1.TLC, r2);
    clip_point_to_rect(r1.BRC, r2);
}

template <typename T>
std::ostream & operator<< (std::ostream & s, const Rect<T> & r)
{
  s << "Rect(" 
    << r.BRC.x-r.TLC.x << "x" << r.BRC.y-r.TLC.y 
    << "+" << r.TLC.x 
    << "+" << r.TLC.y 
    << ")";
  return s;
}

#endif /* RECT_H */
