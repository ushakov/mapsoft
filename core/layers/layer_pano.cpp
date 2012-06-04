#include "layer_pano.h"
#include "2d/rainbow.h"

using namespace std;

LayerPano::LayerPano(srtm3 * s): srtm(s), ray_cache(512) {
  opt.put<int>("pano_width",   3600); // 360deg width in px.
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
LayerPano::set_opt(const Options & o){
  opt = o;
  ray_cache.clear();  // todo -- separate setting for p0, h...
}

Options
LayerPano::get_opt(void) const{ return opt; }

/***********************************************************/

// find segments of the ray brocken by srtm grid
// these segments must have linear height and slope dependence
vector<LayerPano::ray_data>
LayerPano::get_ray(dPoint pt, int x, double max_r){
  double w = opt.get<double>("pano_width");;
  while (x<0) x+=w;
  while (x>=w) x-=w;
  //a is counted clockwise from north in radians
  double a = 2*M_PI*x/w;
  int key=round(a*1000000);

  if (ray_cache.contains(key)) return ray_cache.get(key);


  double sa=sin(a), ca=cos(a), cx=cos(pt.y * M_PI/180.0);
  pt*=1200; // srtm units
  max_r*=srtm_width * 180/M_PI/ 6380000;

  double rx,drx,ry,dry;
  if      (sa>1/max_r) { drx=cx/sa;  rx=(ceil(pt.x)-pt.x)*cx/sa;  }
  else if (sa<-1/max_r){ drx=-cx/sa; rx=(floor(pt.x)-pt.x)*cx/sa; }
  else                 { drx=rx=max_r; }
  if      (ca>1/max_r) { dry=1/ca;  ry=(ceil(pt.y)-pt.y)/ca;  }
  else if (ca<-1/max_r){ dry=-1/ca; ry=(floor(pt.y)-pt.y)/ca; }
  else                 { dry=ry=max_r; }

  vector<LayerPano::ray_data> ret;
//  double max_h=-1000;
  while (rx<max_r || ry<max_r){
    while (rx <= ry){
      int x = round(pt.x+rx*sa/cx);
      double y = pt.y+rx*ca;

      // assert(abs(pt.x+rx*sa/cx - x) < 1e-9);

      int y1 = floor(y);
      int y2 = ceil(y);
      double h,s;
      if (y1!=y2){
        int h1 = srtm->geth(x,y1);
        int h2 = srtm->geth(x,y2);
        double s1 = srtm->slope(x,y1);
        double s2 = srtm->slope(x,y2);
        h = h1 + (h2-h1)*(y-y1)/(y2-y1);
        s = s1 + (s2-s1)*(y-y1)/(y2-y1);
      }
      else {
        h = srtm->geth(x,y1);
        s = srtm->slope(x,y1);
      }
      ret.push_back(ray_data(rx, h, s));
//      if (max_h<h) max_h=h;
      rx+=drx;
    }
    while (ry <= rx){
      double x = pt.x+ry*sa/cx;
      int y = round(pt.y+ry*ca);

      // assert(abs(pt.y+ry*ca - y) < 1e-9);

      int x1 = floor(x);
      int x2 = ceil(x);
      double h,s;
      if (x1!=x2){
        int h1 = srtm->geth(x1,y);
        int h2 = srtm->geth(x2,y);
        double s1 = srtm->slope(x1,y);
        double s2 = srtm->slope(x2,y);
        h = h1 + (h2-h1)*(x-x1)/(x2-x1);
        s = s1 + (s2-s1)*(x-x1)/(x2-x1);
      }
      else {
        h = srtm->geth(x1,y);
        s = srtm->slope(x1,y);
      }
      ret.push_back(ray_data(ry, h, s));
//      if (max_h<h) max_h=h;
      ry+=dry;
    }
  }
//  if (ret.size()) ret[0].h=max_h;
  ray_cache.add(key,ret);
  return ret;
}

/***********************************************************/

int
LayerPano::draw(iImage & image, const iPoint & origin){
  if (!srtm) return GOBJ_FILL_NONE;

  double max_r = opt.get<double>("pano_maxr");
  double w     = opt.get<double>("pano_width");
  double dh    = opt.get<double>("pano_alt");
  double min_h = opt.get<double>("pano_minh");
  double max_h = opt.get<double>("pano_maxh");
  dPoint p0    = opt.get<dPoint>("pano_pt");
  simple_rainbow rb(min_h, max_h);

//  const double RE = 6380000.0;
//  const double d2r = M_PI/180.0;
//  double cc = cos(p0.y * d2r);
  double h0 = (double)srtm->geth4(p0) + dh;

//  int mode=0; // whole view is above terrain

  for (int x=0; x < image.w; x++){

//std::cerr << "XX " << x << "\n";
    // get ray data
    vector<ray_data> ray = get_ray(p0,x+origin.x,max_r);
    if (!ray.size()) continue;
//    int hM = ray[0].h; // max altitude

    int yo = image.h;
    int yp = w/4.0-origin.y;
    double hp, sp;
    for (int i=1; i<ray.size(); i++){
      double hp=ray[i-1].h;
      double sp=ray[i-1].s;
      double hn=ray[i].h;
      double sn=ray[i].s;
      double r=ray[i].r /=1200 * 180/M_PI/ 6380000;
      double b = atan((hn-h0)/r);

      int yn = (1/2.0 - 2*b/M_PI) * w/4.0 - origin.y;
      if (yn<0)  {i=ray.size();}
      if (yn>=yo){yp=yn; continue; }
      for (int y = yn; y < yp; y++){
        if (y<0 || y>=yo) continue;
        double s = sp + (sn-sp)*(y-yp)/double(yn-yp);
        double h = hp + (hn-hp)*(y-yp)/double(yn-yp);
        int color = color_shade(rb.get(h), (1-r/max_r)*(1-s/90));
        image.set_na(x,y, color);
      }
      if (yn<yo) yo=yn;
      yp=yn;
    }

    for (int y = 0; y < yo; y++)
      image.set_na(x,y,0xFFBBBB);
  }
  return GOBJ_FILL_ALL;
}

