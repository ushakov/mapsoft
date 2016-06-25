#include "srtm/srtm3.h"
#include "2d/rainbow.h"
#include "loaders/image_png.h"

const int max_r = 60000; // m
const int min_r = 100;   // m

const double min_b = -M_PI/4.0; // панорама от min_b до max_b по вертикали
const double max_b =  M_PI/4.0; //
const double min_a = -M_PI;   //
const double max_a =  M_PI;   //

const double Re=6380000;


dPoint fast_cnv(double lon0, dPoint p){
  return dPoint((p.x-lon0)*Re*cos(p.y), p.y*Re);
}
dPoint fast_rcnv(double lon0, dPoint p){
  return dPoint(lon0 + p.x/Re/cos(p.y/Re), p.y/Re);
}

int shade(int c, double k){
  unsigned char r=(c>>16)&0xff, g=(c>>8)&0xff,  b=c&0xff;
  r*=k; g*=k; b*=k;
  return (r << 16) + (g << 8) + b;
}

simple_rainbow R(500,3000);

int r2col(double r, double h){
  if (r>max_r) r=max_r;
  return shade(R.get(h), pow(1-(r-min_r)/(max_r-min_r),4));
}

int main(int argc, char *argv[]) {

  SRTM3 s;

  int width   = 1200; // width of picture
  double rad2pt = double(width)/(max_a-min_a);
  double pt2rad = 1.0/rad2pt;
  int height   = (max_b-min_b)*rad2pt;

  double lon0 = 97.0046056;
  double lat0 = 53.9227028;

  double alt0 = s.geth4(dPoint(lon0, lat0), true) + 10;

  lon0 *= M_PI/180.0;
  lat0 *= M_PI/180.0;

  iImage data(width, height);

  // примерный шаг srtm
  int dr = int(1/1200.0 * Re *M_PI/90.0 *cos(lat0));

  dPoint p0 = fast_cnv(lon0, dPoint(lon0, lat0));
  double x0 = p0.x;
  double y0 = p0.y;
  double z0 = alt0;


  for (int x=0; x<width; x++){
    int zo=0;
    bool bnd=false;

    double a = max_a - x*pt2rad;

    for (int i = min_r/dr; i<max_r/dr; i++){
      int r = i*dr;

      dPoint p = fast_rcnv(lon0, dPoint(r*cos(a)+x0, r*sin(a)+y0)) * (180.0/M_PI);
      double alt = (double)s.geth4(p, true);

      int zn = int((atan((alt-alt0)/double(r)) - min_b)*rad2pt);

      if (zn<zo)    bnd=true;
      if (zn<=zo) continue;
      if (zn>=height) zn=height;

      for (int z = zo; z<zn; z++ ) data.set(x,height-z-1, r2col(r, alt));
      if (bnd) data.set(x,height-zo-1, 0);
      zo=zn;
      bnd=false;

      double altx = alt0+r*tan(zn/rad2pt + min_b);
      if ((altx > 10000) || (altx<-1000)) r=max_r;
    }
    for (int z = zo; z<height; z++ ) data.set(x,height-z-1,0xFF0000); 
  }

  image_png::save(data,"srtm_pano.png");
}
