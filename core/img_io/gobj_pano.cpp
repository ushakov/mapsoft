#include "gobj_pano.h"
#include "2d/rainbow.h"

using namespace std;

GObjPano::GObjPano(srtm3 * s): srtm(s), ray_cache(512), rb(0,0){ }

/***********************************************************/
// GET/SET parameters
void
GObjPano::set_origin(const dPoint & p){
  ray_cache.clear(); p0=p;
}
dPoint
GObjPano::get_origin(void) const {return p0;}

void
GObjPano::set_alt(double h) { dh=h;}
double
GObjPano::get_alt(void) const{ return dh;}

void
GObjPano::set_colors(double min, double max){
  rb.set_range(min,max);
}
double
GObjPano::get_minh(void) const {return rb.get_min();}
double
GObjPano::get_maxh(void) const {return rb.get_max();}

void
GObjPano::set_maxr(double r){
  max_r=r;
  ray_cache.clear();
}
double
GObjPano::get_maxr(void) const {return max_r;}

void
GObjPano::set_width(int w){ width0=w; }
int
GObjPano::get_width(void) const {return width0;}

void
GObjPano::set_opt(const Options & o){
  p0 = o.get<dPoint>("pano_pt");
  dh = o.get<double>("pano_alt", 20.0);
  rb.set_range(
    o.get<double>("pano_minh", 0),
    o.get<double>("pano_maxh", 5000));
  max_r  = o.get<double>("pano_maxr", 60000);
  width0 = o.get<int>("pano_width", 3600);
  ray_cache.clear();
}

Options
GObjPano::get_opt(void) const{
  Options o;
  o.put<dPoint>("pano_pt", p0);
  o.put<double>("pano_alt", dh);
  o.put<double>("pano_minh", rb.get_min());
  o.put<double>("pano_maxh", rb.get_max());
  o.put<double>("pano_maxr", max_r);
  o.put<int>("pano_width", width0);
  return o;
}

/***********************************************************/

// find segments of the ray brocken by srtm grid
// these segments must have linear height and slope dependence
vector<GObjPano::ray_data>
GObjPano::get_ray(int x){

  double width=getw();

  while (x<0) x+=width;
  while (x>=width) x-=width;
  double a = 2.0*M_PI*x/width;

  int key=round(a*1000000);
  if (ray_cache.contains(key)) return ray_cache.get(key);

  double sa=sin(a), ca=cos(a), cx=cos(p0.y * M_PI/180.0);
  dPoint pt= p0*1200; // srtm units
  double m2srtm=1200 * 180/M_PI/ 6380000;
  double rM=max_r*m2srtm;

  // Intersections or ray with x and y lines of srtm grid goes
  // with constant steps. But we need to sort them by r.
  double rx,drx,ry,dry; // Initial values and steps
  if      (sa>1/rM) { drx=cx/sa;  rx=(ceil(pt.x)-pt.x)*cx/sa;  }
  else if (sa<-1/rM){ drx=-cx/sa; rx=(floor(pt.x)-pt.x)*cx/sa; }
  else              { drx=rx=rM; }
  if      (ca>1/rM) { dry=1/ca;  ry=(ceil(pt.y)-pt.y)/ca;  }
  else if (ca<-1/rM){ dry=-1/ca; ry=(floor(pt.y)-pt.y)/ca; }
  else                 { dry=ry=rM; }

  vector<GObjPano::ray_data> ret;
  while (rx<rM || ry<rM){ // Go from zero to rM

    while (rx <= ry && rx<rM){ // step in x
      int x = round(pt.x+rx*sa/cx); // x is a round number
      double y = pt.y+rx*ca;        // y is between two grid points.
      // assert(abs(pt.x+rx*sa/cx - x) < 1e-9); // check this

      // Interpolate altitude and slope between two points:
      int y1 = floor(y);
      int y2 = ceil(y);
      double h=srtm_undef,s;
      if (y1!=y2){
        int h1 = srtm->geth(x,y1, false);
        int h2 = srtm->geth(x,y2, false);
        double s1 = srtm->slope(x,y1, false);
        double s2 = srtm->slope(x,y2, false);
        if ((h1>srtm_min) && (h2>srtm_min))
          h = h1 + (h2-h1)*(y-y1)/(y2-y1);
        s = s1 + (s2-s1)*(y-y1)/(y2-y1);
      }
      else {
        h = srtm->geth(x,y1, false);
        s = srtm->slope(x,y1, false);
      }
      // Save data and do step:
      if  (h>srtm_min) ret.push_back(ray_data(rx/m2srtm, h, s));
      rx+=drx;
    }
    while (ry <= rx && ry<rM){ // step in y; the same but rx->ry, y is on the grid
      double x = pt.x+ry*sa/cx;
      int y = round(pt.y+ry*ca);
      // assert(abs(pt.y+ry*ca - y) < 1e-9);

      int x1 = floor(x);
      int x2 = ceil(x);
      double h=srtm_undef,s;
      if (x1!=x2){
        int h1 = srtm->geth(x1,y, false);
        int h2 = srtm->geth(x2,y, false);
        double s1 = srtm->slope(x1,y, false);
        double s2 = srtm->slope(x2,y, false);
        if ((h1>srtm_min) && (h2>srtm_min))
          h = h1 + (h2-h1)*(x-x1)/(x2-x1);
        s = s1 + (s2-s1)*(x-x1)/(x2-x1);
      }
      else {
        h = srtm->geth(x1,y, false);
        s = srtm->slope(x1,y, false);
      }
      if  (h>srtm_min) ret.push_back(ray_data(ry/m2srtm, h, s));
      ry+=dry;
    }
  }
  ray_cache.add(key,ret);
  return ret;
}

/***********************************************************/

iPoint
GObjPano::geo2xy(const dPoint & pt){

  double width=getw();

  iPoint ret;
  double cx=cos(p0.y*M_PI/180);
  ret.x = width * atan2((pt.x-p0.x)*cx, pt.y-p0.y)/2.0/M_PI;
  double r0 = hypot((pt.x-p0.x)*cx, pt.y-p0.y)*6380000/180.0*M_PI;

  vector<ray_data> ray = get_ray(ret.x);
  if (!ray.size()) return iPoint();

  int yp, yo;
  yo=yp=width/2.0;
  double h0 = ray[0].h+dh;
  double rp = 0;

  for (int i=0; i<ray.size(); i++){
    double hn=ray[i].h;
    double rn=ray[i].r;

    double b = atan2(hn-h0, rn); // vertical angle
    int yn = (1 - 2*b/M_PI) * width/4.0; // y-coord

    if (rn>r0){
      ret.y = yp + (yn-yp)*(rn-r0)/(rn-rp);
      if (ret.y > yo) ret.y = -ret.y; //invisible
      return ret;
    }
    if (yn<yo) yo=yn;
    yp=yn;
    rp=rn;
  }

  ret.y = 0;
  return ret;
}


dPoint
GObjPano::xy2geo(const iPoint & pt){

  double width=getw();

  vector<ray_data> ray = get_ray(pt.x);
  if (!ray.size()) return iPoint(0,180);

  double h0 = ray[0].h+dh;
  int yp, yo;
  yo=yp=width/2.0;
  double rp = 0;
  for (int i=0; i<ray.size(); i++){
    double hn=ray[i].h;
    double rn=ray[i].r;

    double b = atan2(hn-h0,rn); // vertical angle
    int yn = (1 - 2*b/M_PI) * width/4.0; // y-coord

    if (yn<pt.y){
      double r = rp + (rn-rp)*(pt.y-yn)/double(yp-yn);
      double a = pt.x/double(width)*2*M_PI;
      double cx=cos(p0.y*M_PI/180.0);
      return p0 + dPoint(sin(a)/cx, cos(a))*r / 6380000.0 / M_PI * 180.0;
    }

    if (yn<yo) yo=yn;
    yp=yn;
    rp=rn;
  }
  return dPoint(0,180);
}

/***********************************************************/

int
GObjPano::draw(iImage & image, const iPoint & origin){
  if (!srtm) return GOBJ_FILL_NONE;

  double width=getw();

  double h0 = (double)srtm->geth4(p0) + dh; // altitude of observation point
  for (int x=0; x < image.w; x++){
    // get ray data -- r,h,s values for a giver x-coord
    vector<ray_data> ray = get_ray(x+origin.x);
    if (!ray.size()) continue;

    int yo = image.h;        // Old y-coord, previously painted point.
                             // It is used to skip hidden parts.
    int yp = width/2.0-origin.y; // Previous value, differs from yo on hidden 
                             // and partially hydden segments.
                             // It is used to interpolate height and slope.
                             // Y axis goes from top to buttom!
    for (int i=1; i<ray.size(); i++){
      double hp=ray[i-1].h;  // altitudes and slopes at the end of segment
      double sp=ray[i-1].s;
      double hn=ray[i].h;
      double sn=ray[i].s;
      double r=ray[i].r;
      if (r>max_r) break;

      double b = atan2(hn-h0, r); // vertical angle
      int yn = (1 - 2*b/M_PI) * width/4.0 - origin.y; // y-coord

      if (yn<0)  {i=ray.size();}     // above image -- draw the rest of segment and exit
      if (yn>=yo) {yp=yn; continue;} // point below image -- skip segment

      for (int y = yn; y < yp; y++){
        if (y<0 || y>=yo) continue; // select visible points
        double s = sp + (sn-sp)*(y-yp)/double(yn-yp); // Interpolate slope and altitude
        double h = hp + (hn-hp)*(y-yp)/double(yn-yp); //  and calculate point color.
        int color = color_shade(rb.get(h), (1-r/max_r)*(1-s/90));
        image.set_na(x,y, color);
      }
      if (yn<yo) yo=yn;
      yp=yn;
    }

    for (int y = 0; y < yo; y++) // dray sky points
      image.set_na(x,y,0xFFBBBB);
  }

  return GOBJ_FILL_ALL;
}


