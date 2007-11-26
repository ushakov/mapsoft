#ifndef POINT_H
#define POINT_H

#include <boost/operators.hpp>
//#include <boost/test/floating_point_comparison.hpp>
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

  operator Point<double>() const{
    return Point<double>(double(this->x), double(this->y));
  }

  operator Point<int>() const{
    return Point<int>(int(this->x), int(this->y));
  }

//  bool equals (Point<T> const & other) const {
//      static boost::test_tools::close_at_tolerance<T> cls (1e-9);
//      return cls(x, other.x) && cls(y, other.y);
//  }

  // Доступ к 8 соседним точкам
  Point<int> adjacent(int dir) const {
    switch(dir%8){
      case 0: return Point<int>(x-1,y-1);
      case 1: return Point<int>(  x,y-1);
      case 2: return Point<int>(x+1,y-1);
      case 3: return Point<int>(x+1,  y);
      case 4: return Point<int>(x+1,y+1);
      case 5: return Point<int>(  x,y+1);
      case 6: return Point<int>(x-1,y+1);
      case 7: return Point<int>(x-1,  y);
    }
  }
  // Проверка, являются ли точка соседней.
  // Если да, то возвращается направление на нее,
  // если нет, то -1
  int isadjacent(Point<int> p){
    for (int i = 0; i<8; i++){
      if (adjacent(i)==p) return i;
    } return -1;
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


// пересечение прямых, задаваемых отрезками p1-p2 и p3-p4.
// для точки пересечения p возвращаются два числа:
// |p-p1|/|p2-p1| и |p-p3|/|p4-p3|
// если прямые параллельны - возникает исключение.

template <typename T>
Point<double> find_cross_ab(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<T> p12 = p2-p1;
  Point<T> p34 = p4-p3;
  Point<T> p13 = p3-p1;
  double k = p12.x*p34.y - p12.y*p34.x;
  double ka = p13.x*p34.y - p13.y*p34.x;
  double kb = p13.x*p12.y - p13.y*p12.x;
  if (k==0) throw 0; // параллельные линии(?)
  return Point<double>(ka/k, kb/k);
}

// Пересечение двух отрезков
// Отрезок включает первую точку, но не включает последнюю
// Если отрезки не пересекаются - возникает исключение

template <typename T>
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<double> ab = find_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // пересечение - за пределами отрезка
  if ((ab.y<0)||(ab.y>=1)) throw 0; // пересечение - за пределами отрезка
  return Point<T>((T)(p1.x+ab.x*(p2.x-p1.x)), (T)(p1.y+ab.x*(p2.y-p1.y)));
}




template <typename T>
std::ostream & operator<< (std::ostream & s, const Point<T> & p)
{
  s << "Point(" << p.x << "," << p.y << ")";
  return s;
}

#endif /* POINT_H */
