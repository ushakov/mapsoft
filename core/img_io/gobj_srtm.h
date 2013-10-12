#ifndef GOBJ_SRTM_H
#define GOBJ_SRTM_H

#include "gobj_geo.h"
#include "geo/geo_convs.h"
#include "2d/point.h"
#include "2d/rect.h"
#include "2d/image.h"

#include "srtm/srtm3.h"
#include "options/options.h"

/// SRTM3 data layer.

class GObjSRTM
#ifndef SWIG
  : public GObjGeo
#endif  // SWIG
{
private:
  srtm3 *S;
public:

  GObjSRTM(srtm3* srtm);

  /// Draw on image.
  int draw(iImage & image, const iPoint & origin);

  /// Get layer range.
  iRect range() const{
    return iRect(
      iPoint(INT_MIN, INT_MIN),
      iPoint(INT_MAX, INT_MAX));
  }
};


#endif
