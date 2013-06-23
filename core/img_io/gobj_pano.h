#ifndef GOBJ_PANO_H
#define GOBJ_PANO_H

#include "2d/rainbow.h"
#include "gred/gobj.h"
#include "srtm/srtm3.h"
#include "options/options.h"
#include <vector>

class GObjPano : public GObj{
private:

  srtm3 *srtm;

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

  double getw() const {dPoint p(width0,0); cnv->bck(p); return p.x;}

public:
  GObjPano(srtm3 * s);

  // Horizontal range is 720deg, -width..width
  // Vertical range is 90, 0..width/4
  iRect range() const {double w=getw(); return iRect(0,0,w,w/2);}
  bool get_xloop() const {return true;}

  void set_origin(const dPoint & p); // central point, wgs84 lonlat
  dPoint get_origin(void) const;

  void set_alt(double h); // altitude above terrain
  double get_alt(void) const;

  void set_colors(double min, double max); // rainbow limits
  double get_minh(void) const;
  double get_maxh(void) const;

  void set_maxr(double r); // max distance
  double get_maxr(void) const;

  void set_width(int w); // 360deg width
  int get_width(void) const;

  void set_opt(const Options & o);
  Options get_opt(void) const;

  // find segments of the ray brocken by srtm grid
  // these segments must have linear height and slope dependence
  std::vector<ray_data> get_ray(int x);

  iPoint geo2xy(const dPoint & pt);
  dPoint xy2geo(const iPoint & pt);

  int draw(iImage &img, const iPoint &origin);
};

#endif
