#ifndef LAYER_PANO_H
#define LAYER_PANO_H

#include "2d/rainbow.h"
#include "gred/iface/gobj.h"
#include "srtm/srtm3.h"
#include "options/options.h"
#include <vector>

class LayerPano : public GObj{
private:

  srtm3 *srtm;

  dPoint p0;
  double max_r;
  double dh;
  simple_rainbow rb;
  int width, width0;

  dPoint dest;

  struct ray_data{
    double r, h, s; // distance, height, slope
    ray_data(double r_, double h_, double s_):r(r_),h(h_),s(s_){}
  };

  Cache<int, std::vector<ray_data> > ray_cache;


public:
  LayerPano(srtm3 * s);

  // Horizontal range is 720deg, -width..width
  // Vertical range is 90, 0..width/4
  iRect range() const;


  void set_origin(const dPoint & p); // central point, wgs84 lonlat
  dPoint get_origin(void) const;

  void set_alt(double h); // altitude above terrain
  double get_alt(void) const;

  void set_colors(double min, double max); // rainbow limits
  double get_minh(void) const;
  double get_maxh(void) const;

  void set_maxr(double r); // max distance
  double get_maxr(void) const;

  void set_width0(int w); // 360deg width
  int get_width0(void) const;
  int get_width(void) const;

  void set_scale(const double k);
  double get_scale(void) const;

  void set_opt(const Options & o);
  Options get_opt(void) const;

  // find segments of the ray brocken by srtm grid
  // these segments must have linear height and slope dependence
  std::vector<ray_data> get_ray(dPoint pt, int x, double max_r);
  iPoint geo2xy(const dPoint & pt);
  void set_dest(const dPoint & pt);


  int draw(iImage &img, const iPoint &origin);
};

#endif