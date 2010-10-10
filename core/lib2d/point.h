#ifndef POINT_H
#define POINT_H

#include <boost/operators.hpp>
#include <ios>   // for << and >> operators
#include <cmath> // for rint

/// 2-d point
/** Short typedefs:
 *   - iPoint for Point<int>
 *   - dPoint for Point<double>
*/

template <typename T>
struct Point
#ifndef SWIG
    : public boost::additive<Point<T> >,
    public boost::multiplicative<Point<T>,T>,
    public boost::less_than_comparable<Point<T> >,
    public boost::equality_comparable<Point<T> >
#endif
{
  /// x coordinate
  T x;
  /// y coordinate
  T y; 

  /// Constructor.
  Point(T _x=0, T _y=0)
	: x(_x), y(_y)
  { }

  /// Swap point with other one.
  void
  swap (Point & other)
  {
	std::swap (x, other.x);
	std::swap (y, other.y);
  }

  /// Subtract corresponding coordinates: p=Point(p.x-other.x, p.y-other.y).
  Point<T> & operator-= (Point const & other)
  {
	x -= other.x;
	y -= other.y;
	return *this;
  }

  /// Add corresponding coordinates: p=Point(p.x+other.x, p.y+other.y).
  Point<T> & operator+= (Point const & other)
  {
	x += other.x;
	y += other.y;
	return *this;
  }

  /// Divide coordinates by k: p=Point(p.x/k, p.y/k).
  Point<T> & operator/= (T k)
  {
	x /= k;
	y /= k;
	return *this;
  }

  /// Multiply coordinates by k: p=Point(p.x*k, p.y*k).
  Point<T> & operator*= (T k)
  {
	x *= k;
	y *= k;
	return *this;
  }

  /// Invert point coordinates.
  Point<T> & operator- ()
  {
	x = -x;
	y = -y;
	return *this;
  }

#ifndef SWIG
  /// Less then operator: (x < other.x)  || ((x == other.x) && (y < other.y)).
  bool operator< (const Point<T> & other) const
  {
	return (x<other.x) || ((x==other.x)&&(y<other.y));
  }

  /// Equal opertator: (x==other.x) && (y==other.y).
  bool operator== (const Point<T> & other) const
  {
	return (x==other.x)&&(y==other.y);
  }

  /// Cast to Point<double>.
  operator Point<double>() const{
    return Point<double>(double(this->x), double(this->y));
  }

  /// Cast to Point<int>.
  operator Point<int>() const{
    return Point<int>(int(rint(this->x)), int(rint(this->y)));
  }
#else  // SWIG
  // Replace boost added standalone operators.
  %extend {
    Point<T> operator+ (Point<T> const & t) { return *$self + t; }
    Point<T> operator- (Point<T> const & t) { return *$self - t; }
    Point<T> operator/ (T k) { return *$self / k; }
    Point<T> operator* (T k) { return *$self * k; }
    swig_cmp(Point<T>);
    swig_str();
  }
#endif  // SWIG

  /// Calculate manhattan length: abs(x)+abs(y).
  T manhattan_length () const
  {
	return abs(x) + abs(y);
  }

};

/// Output operator: print point as a comma-separated pair of coordinartes.
template <typename T>
std::ostream & operator<< (std::ostream & s, const Point<T> & p){
  s << p.x << "," << p.y;
  return s;
}

/// Input operator: read point from a comma-separated pair of coordainates.
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

typedef Point<double> dPoint;
typedef Point<int>    iPoint;

#endif /* POINT_H */
