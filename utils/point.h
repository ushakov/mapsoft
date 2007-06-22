#ifndef POINT_H
#define POINT_H

#include <boost/operators.hpp>
#include <iostream>
#include <cmath>

template <typename T> 
struct Point
  : public boost::additive<Point<T> >,
    public boost::multiplicative<Point<T>,T>,
    public boost::less_than_comparable<Point<T> >,
    public boost::equality_comparable<Point<T> >
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

  bool operator< (const Point<T> & p) const
  {
	return (x<p.x) || ((x==p.x)&&(y<p.y));
  }

  bool operator== (const Point<T> & p) const
  {
	return (x==p.x)&&(y==p.y);
  }

};

template <typename T>
double pscal(const Point<T> & p1, const Point<T> & p2){
  return p1.x*p2.x + p1.y*p2.y;
}
template <typename T>
double pdist(const Point<T> & p){
  return sqrt((double)(pscal(p,p)));
}
template <typename T>
double pdist(const Point<T> & p1, const Point<T> & p2){
  return pdist(p1-p2);
}
template <typename T>
Point<double> pnorm(const Point<T> & p){
  double l = pdist(p);
  return Point<double>(double(p.x)/l, double(p.y/l));
}




template <typename T>
std::ostream & operator<< (std::ostream & s, const Point<T> & p)
{
  s << "Point(" << p.x << "," << p.y << ")";
  return s;
}

#endif /* POINT_H */
