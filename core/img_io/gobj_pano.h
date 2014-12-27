#ifndef GOBJ_PANO_H
#define GOBJ_PANO_H

#include "2d/rainbow.h"
#include "2d/conv_triv.h"
#include "gred/gobj.h"
#include "srtm/srtm3.h"
#include "options/options.h"
#include <vector>

class GObjPano : public GObj{
private:

  srtm3 *srtm;
  ConvTriv cnv;

  dPoint p0;
  double max_r;
  double dh;
  simple_rainbow rb;
  int width0;

  struct ray_data{
    double r, h, s; // distance, height, slope
    ray_data(double r_, double h_, double s_):r(r_),h(h_),s(s_){}
  };

  Cache<int, std::vector<ray_data> > ray_cache;

public:
  GObjPano(srtm3 * s);

  // Horizontal range is 720deg, -width..width
  // Vertical range is 90, 0..width/4
  iRect range() const {int w=get_width(); return iRect(0,0,w,w/2);}
  bool get_xloop() const {return true;}

   // central point, wgs84 lonlat
  void set_origin(const dPoint & p) {ray_cache.clear(); p0=p;}
  dPoint get_origin(void) const {return p0;}

  // altitude above terrain
  void set_alt(double h) { dh=h;}
  double get_alt(void) const { return dh;}

  // rainbow limits
  void set_colors(double min, double max) {rb.set_range(min,max);}
  double get_minh(void) const {return rb.get_min();}
  double get_maxh(void) const {return rb.get_max();}

  // max distance
  void set_maxr(double r) {max_r=r; ray_cache.clear();}
  double get_maxr(void) const {return max_r;}

  // 360deg width
  void set_width(int w) { width0=w; }
  int get_width(void) const {dPoint p(width0,0); cnv.bck(p); return p.x;}

  void set_opt(const Options & o);
  Options get_opt(void) const;

  // find segments of the ray brocken by srtm grid
  // these segments must have linear height and slope dependence
  std::vector<ray_data> get_ray(int x);

  iPoint geo2xy(const dPoint & pt);
  dPoint xy2geo(const iPoint & pt);

  int draw(iImage &img, const iPoint &origin);

  void rescale(double k){cnv.rescale_src(k);}
};

#endif
