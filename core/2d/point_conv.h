#ifndef POINT_CONV_H
#define POINT_CONV_H

#include "point.h"
#include "line.h"
#include "rect.h"
#include <boost/operators.hpp>

// common Conv for point conversions
struct Conv
  : public boost::additive<dPoint>,
    public boost::multiplicative<Conv, double>
{

  virtual void frw(dPoint & p) const =0;
  virtual void bck(dPoint & p) const =0;

  // convert a line, point by point
  virtual void line_frw_p2p(dLine & l) const;
  virtual void line_bck_p2p(dLine & l) const;

  // Convert a line. Each segment can be divided to provide
  // accuracy <acc> in source units. <max> is a maximum number
  // of divisions.
  virtual dLine line_frw(const dLine & l, double acc=1, int max=10) const;
  virtual dLine line_bck(const dLine & l, double acc=1, int max=10) const;

  // convert a rectagle and return bounding box of resulting figure
  virtual dRect bb_frw(const dRect & R, double acc=1, int max=100) const;
  virtual dRect bb_bck(const dRect & R, double acc=1, int max=100) const;
};

// simple conversion: shift and scale
// NOT USED AND NOT TESTED!
struct SimpleConv: public Conv {

  dPoint shift;
  double scale;

  SimpleConv & operator-= (dPoint const & s);
  SimpleConv & operator+= (dPoint const & s);
  SimpleConv & operator*= (double const k);
  SimpleConv & operator/= (double const k);

  void frw(dPoint & p);
  void bck(dPoint & p);

  virtual dLine line_frw(const dLine & l, double acc=1, int max=100);
  virtual dLine line_bck(const dLine & l, double acc=1, int max=100);

  virtual dRect bb_frw(const dRect & R, double acc=1, int max=100);
  virtual dRect bb_bck(const dRect & R, double acc=1, int max=100);
};

#endif /* POINT_CONV_H */
