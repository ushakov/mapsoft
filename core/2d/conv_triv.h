#ifndef CONV_TRIV_H
#define CONV_TRIV_H

#include "conv.h"
#include <map>

///\addtogroup lib2d
///@{
///\defgroup conv_triv
///@{

/// Trivial transformation with scaling
class ConvTriv : public Conv {
  double scale;
public:
  /// constructor - trivial transformation
  ConvTriv(double sc=1.0):scale(sc){}

  void frw(dPoint & p) const {p*=scale;}
  void bck(dPoint & p) const {p/=scale;}

  void rescale_src(const double sc) { scale/=sc; }
  void rescale_dst(const double sc) { scale*=sc; }
};

#endif
