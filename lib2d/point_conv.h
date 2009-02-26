#ifndef POINT_CONV_H
#define POINT_CONV_H

#include <boost/operators.hpp>

// interface for point conversions

struct PointConv
  : public boost::additive<Point<double> >,
    public boost::multiplicative<PointConv, double>,
{
  // shifts and rescales
  virtual PointConv & operator-= (Point<double> const & s) = 0;
  virtual PointConv & operator+= (Point<double> const & s) = 0;
  virtual PointConv & operator/= (double k) = 0;
  virtual PointConv & operator*= (double k) = 0;

  virtual void frw(Point<double> & p) = 0;
  virtual void bck(Point<double> & p) = 0;
  virtual void frw_safe(Point<double> & p) = 0;
  virtual void bck_safe(Point<double> & p) = 0;
  // convert a line
  // number of points can increase
  // accuracy is in source coordinates
  virtual Line<double> line_frw(const Line<double> & l, double acc=1, int max=100) = 0;
  virtual Line<double> line_bck(const Line<double> & l, double acc=1, int max=100) = 0;

  // convert a rectagle and return bounding box of resulting figure
  virtual Rect<double> bb_frw(const Rect<double> & R, double acc=1){
    g_line v2 = line_frw(rect2line(R), acc);
    return v2.range();
  }
  virtual Rect<double> bb_bck(const Rect<double> & R, double acc=1){
    g_line v2 = line_bck(rect2line(R),acc);
    return v2.range();
  }

};

namespace convs{
struct simple: public PointConv
  Point<double> shift;
  double scale;
  simple & operator-= (Point<double> const & s) {shift-=s;}
  simple & operator+= (Point<double> const & s) {shift+=s;}
  simple & operator*= (double k) {scale*=k;}
  simple & operator/= (double k) {scale/=k;}
};
}//namespace

#endif /* POINT_CONV_H */
