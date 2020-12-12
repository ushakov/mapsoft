#ifndef LINE_TESTS_H
#define LINE_TESTS_H

#include "line.h"
#include <algorithm>

///\addtogroup lib2d
///@{
///\defgroup line_tests
///@{

/// Find sorted coordinates of crossings of y=y0 and line l
/// (or of x=x0 and line l if horiz == false)
/// Line is always treated as closed.
template <typename T>
class LineTester{
  std::vector<Point<T> > sb,se;
  std::vector<double> ss;
  bool horiz;
public:
  LineTester(const Line<T> & l, bool horiz_ = true): horiz(horiz_){
    // collect line sides info: start and end points, slopes.
    int pts = l.size();
    for (int i = 0; i < pts; i++){
      Point<T> b(l[i%pts].x, l[i%pts].y),
               e(l[(i+1)%pts].x, l[(i+1)%pts].y);
      if (!horiz){ // swap x and y
        b.y = l[i%pts].x; b.x=l[i%pts].y;
        e.y = l[(i+1)%pts].x;
        e.x = l[(i+1)%pts].y;
      }
      if (b.y == e.y) continue; // no need for horisontal sides
      double s = double(e.x-b.x)/double(e.y-b.y); // side slope
      sb.push_back(b);
      se.push_back(e);
      ss.push_back(s);
    }
  }

  /// get coords of crossings of border with y=const (or x=const if horiz=false) line
  std::vector<T> get_cr(T y){
    std::vector<T> cr;
    for (size_t k = 0; k < sb.size(); k++){
      if ((sb[k].y >  y)&&(se[k].y >  y)) continue; // side is above the row
      if ((sb[k].y <= y)&&(se[k].y <= y)) continue; // side is below the row
      cr.push_back((ss[k] * double(y - sb[k].y)) + sb[k].x);
    }
    sort(cr.begin(), cr.end());
    return cr;
  }

  /// is coord x inside border at cr line
  bool test_cr(const std::vector<T> & cr, T x) const{
    // number of crossings on the ray (x,y) - (inf,y)
    typename std::vector<T>::const_iterator i =
             lower_bound(cr.begin(), cr.end(), x);
    int k=0;
    while (i!=cr.end()) {i++; k++;}
    return k%2==1;
  }

};
/// \relates LineTester
/// \brief LineTester with double coordinates
typedef LineTester<double> dLineTester;
/// \relates LineTester
/// \brief LineTester with int coordinates
typedef LineTester<int>    iLineTester;


/// Check that point p is inside polygon with boundary l
template <typename T>
bool
point_in_line(const Point<T> & p, const Line<T> & l){
  LineTester<T> lt(l);
  std::vector<T> cr = lt.get_cr(p.y);
  return lt.test_cr(cr, p.x);
}

/// Check that rectangle r touches polygon with boundary l
template <typename T>
bool
rect_in_line(const Rect<T> & r, const Line<T> & l){
  LineTester<T> lth(l, true), ltv(l,false);
  // check if there is any crossing at any sides,
  // or one corner is inside polygon
  // or one of line points is inside rect
  std::vector<T> cr;
  cr = lth.get_cr(r.y);
  for (size_t i=0; i<cr.size(); i++)
    if (r.x < cr[i] && cr[i] <=r.x+r.w ) return true;
  cr = lth.get_cr(r.y+r.h);
  for (size_t i=0; i<cr.size(); i++)
    if (r.x < cr[i] && cr[i] <=r.x+r.w ) return true;
  cr = ltv.get_cr(r.x);
  for (size_t i=0; i<cr.size(); i++)
    if (r.y < cr[i] && cr[i] <=r.y+r.h ) return true;
  cr = ltv.get_cr(r.x+r.w);
  for (size_t i=0; i<cr.size(); i++)
    if (r.y < cr[i] && cr[i] <=r.y+r.h ) return true;

  for (size_t i=0; i<cr.size(); i++){
    if (r.y < cr[i] ){
      if (i%2 == 1) return true;
      break;
    }
  }

  if (l.size() && point_in_rect(l[0], r)) return true;
  return false;
}

#endif
