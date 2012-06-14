#ifndef CONV_AFF_H
#define CONV_AFF_H

#include "conv.h"
#include <map>

///\addtogroup lib2d
///@{
///\defgroup conv
///@{

/// Affine transformation
struct ConvAff : Conv {
private:
  std::vector<double> k_frw;
  std::vector<double> k_bck;
  void bck_recalc(); ///< recalculate k_bck matrix

public:
  /// constructor - trivial transformation
  ConvAff();

  /// constructor - from g_ref
  ConvAff(const std::map<dPoint, dPoint> & ref);

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

#endif
