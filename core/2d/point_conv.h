#ifndef POINT_CONV_H
#define POINT_CONV_H

#include "point.h"
#include "line.h"
#include "rect.h"
#include "image.h"
#include <map>

///\addtogroup lib2d
///@{
///\defgroup point_conv
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

/// Affine transformation
struct AffConv : Conv {
private:
  std::vector<double> k_frw;
  std::vector<double> k_bck;
  void bck_recalc(); ///< recalculate k_bck matrix

public:
  /// constructor - trivial transformation
  AffConv();

  /// constructor - from g_ref
  AffConv(const std::map<dPoint, dPoint> & ref);

  /// reset from g_ref
  void set_from_ref(const std::map<dPoint, dPoint> & ref);

  void frw(dPoint & p) const;
  void bck(dPoint & p) const;

  /// reset to trivial
  void reset();

  void shift(const dPoint & p);
  void scale(const double kx, const double ky);

  void rescale_src(const double s);
  void rescale_dst(const double s);

};

#endif /* POINT_CONV_H */
