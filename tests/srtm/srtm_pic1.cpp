#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* srtm3 example - draw simple picture */

// convert DMS to srtm points
#define DMS(x,y,z) ((x)*3600+(y)*60+z)/3

const int lat1 = DMS(53,45,00); // picture corners
const int lat2 = DMS(54,15,00);
const int lon1 = DMS(95,30,00);
const int lon2 = DMS(96,30,00);

int lighten(int c, double k){
  unsigned char r=(c>>16)&0xff, g=(c>>8)&0xff,  b=c&0xff;
  r=255-(255-r)*k;
  g=255-(255-g)*k;
  b=255-(255-b)*k;
  return (r << 16) + (g << 8) + b;
}

struct rainbow_data RD[]={
  {30.0, 0xFFFFFF},
  {35.0, 0x00FFFF},
  {40.0, 0x0000FF},
  {45.0, 0xFF00FF},
  {50.0, 0xFF0000},
  {55.0, 0x000000}
};
int RDS=sizeof(RD)/sizeof(rainbow_data);

int
main(){

  SRTM3 s;
  simple_rainbow R(500,3000);
  iImage img(lon2-lon1, lat2-lat1);
  iPoint p;

  for (p.y=lat2-1; p.y>=lat1; p.y--){
    for (p.x=lon1; p.x<lon2; p.x++){

      short h = s.geth(p, true);
      double a = s.slope(p);

      int c = get_rainbow(a, RD, RDS);
      img.set(p.x-lon1, lat2-p.y-1, c);
    }
  }
  image_png::save(img, "srtm_pic1.png");
  return 0;
}
