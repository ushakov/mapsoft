#ifndef POINT_H
#define POINT_H

#include <boost/operators.hpp>
#include <iostream>

namespace Point{

  template <typename T> 
  struct Point
    : public boost::additive<Point<T> >,
      public boost::multiplicative<Point<T>,T>,
      public boost::less_than_comparable<Point<T> >,
      public boost::equality_comparable<Point<T> >
  {
    T x, y;

    Point(T _x, T _y)
	: x(_x), y(_y)
    { }
    
    Point()
	: x(0), y(0)
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
std::ostream & operator<< (std::ostream & s, const Point<T> & p)
{
  s << "Point(" << p.x << "," << p.y << ")";
  return s;
}

} //namespace
#endif /* POINT_H */
