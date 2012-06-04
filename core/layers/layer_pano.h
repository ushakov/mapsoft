#ifndef LAYER_PANO_H
#define LAYER_PANO_H

#include "gred/iface/gobj.h"
#include "srtm/srtm3.h"
#include "options/options.h"
#include <vector>

class LayerPano : public GObj{
private:

  double alt;
  srtm3 *srtm;
  Options opt;

  struct ray_data{
    double r, h, s; // distance, height, slope
    ray_data(double r_, double h_, double s_):r(r_),h(h_),s(s_){}
  };

  Cache<int, std::vector<ray_data> > ray_cache;

  // find segments of the ray brocken by srtm grid
  // these segments must have linear height and slope dependence
  std::vector<ray_data> get_ray(dPoint pt, int x, double max_r);

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
