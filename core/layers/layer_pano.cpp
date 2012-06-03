#include "layer_pano.h"
#include "2d/rainbow.h"

using namespace std;

LayerPano::LayerPano(srtm3 * s): srtm(s) {
  opt.put<int>("pano_width",   3600);  // 360deg width in px.
  opt.put<double>("pano_minh", 0.0);
  opt.put<double>("pano_maxh", 6000.0);
  opt.put<double>("pano_alt",  20.0);
  opt.put<double>("pano_minr", 100.0);
  opt.put<double>("pano_maxr", 60000.0);
  opt.put<dPoint>("pano_pt",   dPoint());
}

iRect
LayerPano::range() const {
  int w=opt.get<int>("pano_width",0);
  return iRect(-w,0, 2*w,w/4);
}

void
LayerPano::set_scale(const double k){
  opt.put<int>("pano_width", 3600*k);
}


void
LayerPano::set_opt(const Options & o){ opt = o; }

Options
LayerPano::get_opt(void) const{ return opt; }


int
shade(int c, double k);

int
LayerPano::draw(iImage & image, const iPoint & origin){
  if (!srtm) return GOBJ_FILL_NONE;

  double min_r = opt.get<double>("pano_minr");
  double max_r = opt.get<double>("pano_maxr");
  double w     = opt.get<double>("pano_width");
  double dh    = opt.get<double>("pano_alt");
  double min_h = opt.get<double>("pano_minh");
  double max_h = opt.get<double>("pano_maxh");
  dPoint p0    = opt.get<dPoint>("pano_pt");
  simple_rainbow rb(min_h, max_h);

  const double RE = 6380000.0;
  const double d2r = M_PI/180.0;
  double cc = cos(p0.y * d2r);
  double alt0 = (double)srtm->geth4(p0) + dh;

  int mode=0; // whole view is above terrain

  for (int x=0; x < image.w; x++){
    double a = (x+origin.x) * 2*M_PI/w;
    int yo=image.h;

    double r = min_r;
    while (r < max_r){
      if (mode == 0){
        r += max(
          1/1200.0 * RE * d2r * cc, // srtm_point
          r * 2*M_PI / w ); // screen point
      }
      else {
        r += r * 2*M_PI / w; // screen point
      }

      dPoint p = p0 +
        dPoint(sin(a)/cc, cos(a)) * r / RE / d2r;

      double alt = (double)srtm->geth4(p);
      if (alt > srtm_min_interp) alt-=srtm_zer_interp;

      double b = atan((alt-alt0)/r);
      double altx = alt0 +
        r*tan((1/2.0-(yo+origin.y)/w*4.0)*M_PI/2);
      if ((altx > 10000) || (altx<-1000)) r=max_r;

      int yn = (1/2.0 - 2*b/M_PI) * w/4.0 - origin.y;
      if (yn>=yo) continue; else mode=1;

      // if you use layer_pano and layer_srtm from different threads
      // you can't use interpolation in both places!
      int color = shade(
        rb.get(alt), (1-r/max_r)*(1-srtm->slope4(p, false)/90.0));

      if (yn<0) {yn=0; r=max_r;}

      for (int y = yo-1; y>=yn; y--)
        image.set_na(x,y, color);

      yo=yn;

    }
    for (int y = yo-1; y>=0; y--)
      image.set_na(x,y,0xFFBBBB);
  }
  return GOBJ_FILL_ALL;
}

