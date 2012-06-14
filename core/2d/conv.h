#ifndef CONV_H
#define CONV_H

#include "point.h"
#include "line.h"
#include "rect.h"
#include "image.h"

///\addtogroup lib2d
///@{
///\defgroup conv
///@{

/// Abstract point transformation.
struct Conv{

  Conv();

  virtual void frw(dPoint & p) const =0;
  virtual void bck(dPoint & p) const =0;

  /// Convert a line, point to point.
  virtual void line_frw_p2p(dLine & l) const;
  virtual void line_bck_p2p(dLine & l) const;

  /// Convert a line. Each segment can be divided to provide
  /// accuracy <acc> in source units. <max> is a maximum number
  /// of divisions.
  virtual dLine line_frw(const dLine & l, double acc=1, int max=10) const;
  virtual dLine line_bck(const dLine & l, double acc=1, int max=10) const;

  /// Convert a rectagle and return bounding box of resulting figure.
  virtual dRect bb_frw(const dRect & R, double acc=1, int max=100) const;
  virtual dRect bb_bck(const dRect & R, double acc=1, int max=100) const;

  /// Convert angle from y=const line at point p.
  /// Radians.
  virtual double ang_frw(dPoint p, double a, double dx) const;
  virtual double ang_bck(dPoint p, double a, double dx) const;
  /// Degrees.
  virtual double angd_frw(dPoint p, double a, double dx) const;
  virtual double angd_bck(dPoint p, double a, double dx) const;

  /// Fill dst_image from src_image
  virtual int image_frw(const iImage & src_img, iImage & dst_img,
                        const iPoint & shift = iPoint(0,0),
                        const double scale = 1.0) const;
  virtual int image_bck(const iImage & src_img, iImage & dst_img,
                        const iPoint & shift = iPoint(0,0),
                        const double scale = 1.0) const;

  /// rescale_dst() here only changes sc_dst parameter.
  /// Childs can use this parameter in frw/bck or redefine rescale_dst()
  virtual void rescale_src(const double s);
  virtual void rescale_dst(const double s);
  double sc_src, sc_dst;
};

#endif
