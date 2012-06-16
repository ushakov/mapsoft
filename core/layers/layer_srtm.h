#ifndef LAYER_SRTM_H
#define LAYER_SRTM_H

#include "layer.h"
#include "geo/geo_convs.h"
#include "2d/point.h"
#include "2d/rect.h"
#include "2d/image.h"

#include "srtm/srtm3.h"
#include "options/options.h"

/// SRTM3 data layer.

class LayerSRTM
#ifndef SWIG
  : public Layer
#endif  // SWIG
{
private:
  srtm3 *S;
  Options opt;
public:

  LayerSRTM(srtm3* srtm);

  void set_opt(const Options & o);
  Options get_opt(void) const;

  /// Get some reasonable reference.
  g_map get_myref() const;

  // Optimized get_image to return empty image outside of bounds.
//  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  /// Get layer range.
  iRect range() const;
  bool get_xloop() const;
};


#endif
