#ifndef LAYER_PANO_H
#define LAYER_PANO_H

#include "gred/iface/gobj.h"
#include "srtm/srtm3.h"
#include "options/options.h"

class LayerPano : public GObj{
private:
  int width;   // 360deg width in px.
               // Vertical range is -45..45, height=width/4
  double alt;
  srtm3 *srtm;
  Options opt;

public:
  LayerPano(srtm3 * s);

  // Horizontal range is 720deg, -width..width
  // Vertical range is 90, 0..width/4
  iRect range() const;

  void set_scale(const double k);

  void set_opt(const Options & o);
  Options get_opt(void) const;

  int draw(iImage &img, const iPoint &origin);
};

#endif
