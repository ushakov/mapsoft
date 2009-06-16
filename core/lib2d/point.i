%{
#include "point.h"
%}

template<typename T>
struct Point {
  T x, y;

  Point(T _x=0, T _y=0);
  void swap (Point & other);
  Point<T> & operator-= (Point<T> const & t);
  Point<T> & operator+= (Point<T> const & t);
  Point<T> & operator/= (T k);
  Point<T> & operator*= (T k);
  T manhattan_length () const;
  bool operator< (const Point<T> & p) const;
  bool operator== (const Point<T> & p) const;
  %extend {
    Point<T> operator+ (Point<T> const & t) { return *$self + t; }
    Point<T> operator- (Point<T> const & t) { return *$self - t; }
    Point<T> operator/ (T k) { return *$self / k; }
    Point<T> operator* (T k) { return *$self * k; }
  }
};

%template(point_i) Point<int>;
%template(point_d) Point<double>;
