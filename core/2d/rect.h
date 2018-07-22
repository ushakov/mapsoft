#ifndef RECT_H
#define RECT_H

#include <boost/operators.hpp>
#include <ios>
#include <iostream>
#include <iomanip>
#include "point.h"

///\addtogroup lib2d
///@{
///\defgroup rect
///@{

/// 2-d rectangle
template <typename T>
class Rect
#ifndef SWIG
    : public boost::multiplicative<Rect<T>,T>,
      public boost::additive<Rect<T>,Point<T> >,
      public boost::less_than_comparable<Rect<T> >,
      public boost::equality_comparable<Rect<T> >
#endif  // SWIG
{
public:

    T x; ///< x coordinate
    T y; ///< y coordinate
    T w; ///< width
    T h; ///< height

    /// Constructor: create rectangle by two opposite corners
    /* You may use any pair of opposite corners in any order.
    */
    Rect (Point<T> p1, Point<T> p2)
	: x(std::min(p1.x, p2.x)), y(std::min(p1.y, p2.y)),
	  w(std::max(p1.x,p2.x) - std::min(p1.x,p2.x)),
	  h(std::max(p1.y,p2.y) - std::min(p1.y,p2.y)) {
    }

    /// Constructor: create rectangle from x,y,w,h values
    /* Print warning if h<0 or w<0.
     * TODO: make positive? throw exception?
    */
    Rect (T _x, T _y, T _w=0, T _h=0)
	: x(_x), y(_y), w(_w), h(_h)
    {
	if ((w<0)||(h<0)) std::cerr 
          << "Rect warning: bad geometry: "
          << w << "x" << h << "+" << x << "+" << y << "\n";
    }
    /// Constructor: zero values
    Rect () : x(0), y(0), w(0), h(0)
    { }

    /// Swap rectangle with other one.
    void swap (Rect & other){
      std::swap (x, other.x);
      std::swap (y, other.y);
      std::swap (w, other.w);
      std::swap (h, other.h);
    }

    /// Test if rectangle is empty : (w<=0) || (h<=0)
    bool empty() const { return w <= 0 || h <= 0; }

    /// Top-left corner
    Point<T> TLC() const { return  Point<T>(x,y);}
    /// Top-right corner
    Point<T> TRC() const { return  Point<T>(x+w,y);}
    /// Bottom-right corner
    Point<T> BRC() const { return  Point<T>(x+w,y+h);}
    /// Bottom-left corner
    Point<T> BLC() const { return  Point<T>(x,y+h);}
    /// Center point
    Point<T> CNT() const { return  Point<T>(x+w/2,y+h/2);}

    /// Divide all coordinates by k
    Rect<T> & operator/= (T k)
    {
        x /= k;
        y /= k;
        w /= k;
        h /= k;
        return *this;
    }

    /// Multiply all coordinates by k
    Rect<T> & operator*= (T k)
    {
        x *= k;
        y *= k;
        w *= k;
        h *= k;
        return *this;
    }

    /// Add p to rectangle coordinates (shift)
    Rect<T> & operator-= (Point<T> p){
        x -= p.x;
        y -= p.y;
        return *this;
    }

    /// Subtract p from rectangle (shift)
    Rect<T> & operator+= (Point<T> p){
        x += p.x;
        y += p.y;
        return *this;
    }

#ifndef SWIG
    /// Less-then operator: compare TLC, then BRC
    bool operator< (const Rect<T> & r) const
    {
        if (TLC()!=r.TLC()) return (TLC()<r.TLC());
        return (BRC()<r.BRC());
    }

    /// Equal operator: all coordinates are equal
    bool operator== (const Rect<T> & r) const
    {
        return (x==r.x)&&(y==r.y)&&(w==r.w)&&(h==r.h);
    }

    /// Cast to Rect<double>
    operator Rect<double>() const{
      return Rect<double>(double(this->x), double(this->y),
                          double(this->w), double(this->h));
    }

    /// Cast to Rect<int>
    operator Rect<int>() const{
      return Rect<int>(int(rint(this->x)), int(rint(this->y)),
                       int(rint(this->w)), int(rint(this->h)));
    }
#else  // SWIG
  %extend {
    Rect<T> operator/ (T k) { return *$self / k; }
    Rect<T> operator* (T k) { return *$self * k; }
    Rect<T> operator+ (Point<T> k) { return *$self + k; }
    Rect<T> operator- (Point<T> k) { return *$self - k; }
    swig_cmp(Rect<T>);
    swig_str();
  }
#endif  // SWIG

};

/// \relates Rect
/// \brief Rect with double coordinates
typedef Rect<double> dRect;

/// \relates Rect
/// \brief Rect with int coordinates
typedef Rect<int>    iRect;


/// \relates Rect
/// \brief Output operator: print Rect as a geometry string: WxH+X+Y
template <typename T>
std::ostream & operator<< (std::ostream & s, const Rect<T> & r){
  // std::showpos don't add + to zeros :(
  s << std::setprecision(9) << r.w << "x" << r.h << std::showpos << r.x << r.y;
  return s;
}

/// \relates Rect
/// \brief Input operator: read Rect from a geometry string
template <typename T>
std::istream & operator>> (std::istream & s, Rect<T> & r){
  char sep;

  s >> r.w >> sep >> r.h;
  if (sep!='x') {
    s.setstate(std::ios::failbit);
    return s;
  }

  s >> sep >> r.x;
  if (sep=='-') r.x=-r.x;
  else if (sep!='+'){
    s.setstate(std::ios::failbit);
    return s;
  }

  s >> sep >> r.y;
  if (sep=='-') r.y=-r.y;
  else if (sep!='+'){
    s.setstate(std::ios::failbit);
    return s;
  }
  if (!s.eof()){
    char c=s.get();
    switch(c){
      case 'G': r*=1000000000; break;
      case 'M': r*=1000000; break;
      case 'k': r*=1000; break;
      case 'm': r/=1000; break;
      case 'u': r/=1000000; break;
      case 'p': r/=1000000000; break;
      case ' ':
      case '\t':
      case '\n': break;
      default:
      s.setstate(std::ios::failbit);
      return s;
    }
  }
  s.setstate(std::ios::goodbit);
  return s;
}

/// \relates Rect
/// \brief Pump rectangle to each side by val value.
template <typename T>
Rect<T> rect_pump (Rect<T> const & R, T val) {
    return Rect<T> (R.x-val, R.y-val, R.w+2*val, R.h+2*val);
}

/// \relates Rect
/// \brief Pump rectangle by vx and vy values.
template <typename T>
Rect<T> rect_pump (Rect<T> const & R, T vx, T vy) {
    return Rect<T> (R.x-vx, R.y-vy, R.w+2*vx, R.h+2*vy);
}

/// \relates Rect
/// \brief Pump rectangle to cover point p.
/** Better name is rect_bounding_box(R,p)?
  * Used only in layer_map.h and layer_wpt.h; 
  */
template <typename T>
Rect<T> rect_pump (Rect<T> & R, Point<T> p) {
    Rect<T> ret = R;
    if      (R.x>p.x)     {ret.x=p.x; ret.w+=R.x-p.x;}
    else if (R.x+R.w<p.x) {           ret.w=p.x-R.x;}
    if      (R.y>p.y)     {ret.y=p.y; ret.h+=R.y-p.y;}
    else if (R.y+R.h<p.y) {           ret.h=p.y-R.y;}
    return ret;
}

/// \relates Rect
///\brief ???
/**
  * \bug what for?
  * \todo remove -- NOT USED?
 */
template <typename T>
Rect<T> rect_pump (Rect<T> const & R, Rect<T> bounds) {
    return Rect<T> (R.x-bounds.BRC().x, R.y-bounds.BRC().y, R.w+bounds.w, R.h+bounds.h);
}

/// \relates Rect
/// \brief Calculate intersection of two rectangles.
/** If rectangles does not intersect, returns empty rectangle
  * with some strange position
  */
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

/// Calculate bounding box of two rectangles.
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

/// \relates Rect
/// \brief Move point to the nearest Rect side or corner.
/** \bug BR PROBLEM - is BRC() inside rect?
  * \todo remove -- NOT USED!
  */
// точка не всегда будет входит в прямоугольник!!! 
template <typename T>
void clip_point_to_rect (Point<T> & p, const Rect<T> & r){
    p.x = std::max (r.x, p.x);
    p.x = std::min (r.x+r.w, p.x);
    p.y = std::max (r.y, p.y);
    p.y = std::min (r.y+r.h, p.y);
// точка не всегда будет входит в прямоугольник!!! Исправить?
}

/// \relates Rect
/// \brief Clip r1 by r2.
/** \todo TODO: replace by rect_intersect?
*/
template <typename T>
void clip_rect_to_rect (Rect<T> & r1, const Rect<T> & r2){
    r1 = rect_intersect(r1,r2);
}

/// \relates Rect
/// \brief Check if point is in rectangle
/** \bug BR PROBLEM!
  */
template <typename T>
bool point_in_rect (const Point<T> & p, const Rect<T> & r){
    return (r.x <= p.x) && (r.x+r.w > p.x) &&
           (r.y <= p.y) && (r.y+r.h > p.y);
}

/**
  * при работе с целыми координатами иногда приятно считать,
  * что rect.BRC не входит в прямоугольник
  * (кстати, исходя из этого сделана и проверка на вхождение точки в прямоугольник)
  */

/// \relates Rect
/// \brief  диапазон плиток, накрывающих данный прямоугольник
/// \todo возможность задавать r и tsize типа double (BRC problem!)
/// \todo использовать rect_pump_to_int()!
/// \todo тесты!

iRect tiles_on_rect(const iRect & r, int tsize);

/// \relates Rect
/// \brief  диапазон плиток, лежащих внутри данного прямоугольника
iRect tiles_in_rect(const iRect & r, int tsize);

/// \relates Rect
/// \brief 
/// два прямоугольника задают преобразование.
/// функция соответствующим образом сдвигает и растягивает третий прямоугольник
/// TODO: used only in clip_rects_for_image_loader -- move from rect.h?
void transform_rect(const iRect & src, const iRect & dst, iRect & r);

/// \relates Rect
/// \brief Функция, нужная для загрузчика картинок.
/// Правильное подрезание краев, выходящих за пределы картинки
/// TODO: move from here?
/// used in layers, loaders, 2d/image_source.h
void clip_rects_for_image_loader(
    const iRect & src_img, iRect & src,
    const iRect & dst_img, iRect & dst);

/// \relates Rect
/// \brief  растянуть прямоугольник до ближайших целых значений
iRect rect_pump_to_int(const dRect & r);

#endif /* RECT_H */
