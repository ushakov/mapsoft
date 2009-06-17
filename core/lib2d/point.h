#ifndef POINT_H
#define POINT_H

#include <boost/operators.hpp>
#include <iostream>
#include <ios>
#include <cmath> // for rint

template <typename T>
struct Point
#ifndef SWIG
    : public boost::additive<Point<T> >,
    public boost::multiplicative<Point<T>,T>,
    public boost::less_than_comparable<Point<T> >,
    public boost::equality_comparable<Point<T> >
#endif
{
  T x, y;

  Point(T _x=0, T _y=0)
	: x(_x), y(_y)
  { }

  void
  swap (Point & other)
  {
	std::swap (x, other.x);
	std::swap (y, other.y);
  }

  Point<T> & operator-= (Point const & t)
  {
	x -= t.x;
	y -= t.y;
	return *this;
  }

  Point<T> & operator+= (Point const & t)
  {
	x += t.x;
	y += t.y;
	return *this;
  }

  Point<T> & operator/= (T k)
  {
	x /= k;
	y /= k;
	return *this;
  }

  Point<T> & operator*= (T k)
  {
	x *= k;
	y *= k;
	return *this;
  }

  T manhattan_length () const
  {
	return abs(x) + abs(y);
  }

#ifndef SWIG
  bool operator< (const Point<T> & p) const
  {
	return (x<p.x) || ((x==p.x)&&(y<p.y));
  }

  bool operator== (const Point<T> & p) const
  {
	return (x==p.x)&&(y==p.y);
  }

  operator Point<double>() const{
    return Point<double>(double(this->x), double(this->y));
  }

  operator Point<int>() const{
    return Point<int>(int(rint(this->x)), int(rint(this->y)));
  }
#else  // SWIG
  // replace boost added standalone operators
  %extend {
    Point<T> operator+ (Point<T> const & t) { return *$self + t; }
    Point<T> operator- (Point<T> const & t) { return *$self - t; }
    Point<T> operator/ (T k) { return *$self / k; }
    Point<T> operator* (T k) { return *$self * k; }
    swig_cmp(Point<T>);
    swig_str();
  }
#endif  // SWIG    
};

template <typename T>
std::ostream & operator<< (std::ostream & s, const Point<T> & p){
  s << p.x << "," << p.y;
  return s;
}

template <typename T>
std::istream & operator>> (std::istream & s, Point<T> & p){
  char sep;
  s >> std::ws >> p.x >> std::ws >> sep;
  if (sep!=','){
    s.setstate(std::ios::failbit);
    return s;
  }
  s >> std::ws >> p.y >> std::ws;
  s.setstate(std::ios::goodbit);
  return s;
}

#endif /* POINT_H */
