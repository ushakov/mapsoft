#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* srtm3 example: show interpolation */

#define DMS(x,y,z) ((x)*3600+(y)*60+z)/3

int
main(){

  int lon1 = DMS(96,32,00)*10;
  int lon2 = DMS(96,36,00)*10;
  int lat1 = DMS(53,32,00)*10;
  int lat2 = DMS(53,34,00)*10;

  SRTM3 S;
  simple_rainbow SR(1600,2800);

  iImage i1(lon2-lon1, lat2-lat1);
  iImage i2(lon2-lon1, lat2-lat1);
  iImage i3(lon2-lon1, lat2-lat1);

  iImage i4(lon2-lon1, lat2-lat1);
  iImage i5(lon2-lon1, lat2-lat1);
  iImage i6(lon2-lon1, lat2-lat1);

  for (int x=lon1; x<lon2; x++){
    for (int y=lat1; y<lat2; y++){
      short h1, h2, h3;
      h1=S.geth(round(x/10.0), round(y/10.0));
      h2=S.geth4(dPoint(x,y)/10.0/3600*3);
      h3=S.geth16(dPoint(x,y)/10.0/3600*3);

      i1.set(x-lon1, lat2-y-1, SR.get_bnd(h1));
      i2.set(x-lon1, lat2-y-1, SR.get_bnd(h2));
      i3.set(x-lon1, lat2-y-1, SR.get_bnd(h3));

      h1=S.geth(round(x/10.0), round(y/10.0), true);
      h2=S.geth4(dPoint(x,y)/10.0/3600*3, true);
      h3=S.geth16(dPoint(x,y)/10.0/3600*3, true);

      i4.set(x-lon1, lat2-y-1, SR.get_bnd(h1));
      i5.set(x-lon1, lat2-y-1, SR.get_bnd(h2));
      i6.set(x-lon1, lat2-y-1, SR.get_bnd(h3));
    }
  }

  double x=lon1 + 70;
  for (double y=lat1+200; y<lat1+350; y++){
    short h;
    int y1;
    h = (S.geth(round(x/10.0), round(y/10.0), true)-1000)/10;
    i4.set(x-lon1, lat2-y-1, 0x000000);
    i4.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
    h = (S.geth4(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i5.set(x-lon1, lat2-y-1, 0x000000);
    i5.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
    h = (S.geth16(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i6.set(x-lon1, lat2-y-1, 0x000000);
    i6.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
  }

  x=lon1 + 300;
  for (double y=lat1+150; y<lat1+300; y++){
    short h;
    int y1;
    h = (S.geth(round(x/10.0), round(y/10.0), true)-1000)/10;
    i4.set(x-lon1, lat2-y-1, 0x000000);
    i4.safe_set(y-lat1+200, lat2-lat1-h, 0x000000);
    h = (S.geth4(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i5.set(x-lon1, lat2-y-1, 0x000000);
    i5.safe_set(y-lat1+200, lat2-lat1-h, 0x000000);
    h = (S.geth16(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i6.set(x-lon1, lat2-y-1, 0x000000);
    i6.safe_set(y-lat1+200, lat2-lat1-h, 0x000000);
  }

  image_png::save(i1, "srtm_interp1.png"); // nearest point
  image_png::save(i2, "srtm_interp2.png"); // bilinear
  image_png::save(i3, "srtm_interp3.png"); // bicubic
  image_png::save(i4, "srtm_interp4.png");
  image_png::save(i5, "srtm_interp5.png");
  image_png::save(i6, "srtm_interp6.png");

  /// with contours
  iImage i7(lon2-lon1, lat2-lat1);
  iImage i8(lon2-lon1, lat2-lat1);

  int step=30;
  for (int x=lon1; x<lon2; x++){
    int ho1=srtm_undef, ho2=srtm_undef, ho3=srtm_undef;
    for (int y=lat1; y<lat2; y++){
      short h2=S.geth4(dPoint(x,y)/10.0/3600*3, true);
      short h2x=S.geth4(dPoint(x+1,y)/10.0/3600*3, true);
      short h3=S.geth16(dPoint(x,y)/10.0/3600*3, true);
      short h3x=S.geth16(dPoint(x+1,y)/10.0/3600*3, true);

      i7.set(x-lon1, lat2-y-1, SR.get_bnd(h2));
      i8.set(x-lon1, lat2-y-1, SR.get_bnd(h3));

      int d1,d2;

      d1 = (h2/step - ho2);
      d2 = (h2x/step - ho2);
      ho2 = (abs(d1)>abs(d2))? h2x/step:h2/step;
      if ((d1!=0)||(d2!=0)) i7.set(x-lon1, lat2-y-1, 0);

      d1 = (h3/step - ho3);
      d2 = (h3x/step - ho3);
      ho3 = (abs(d1)<abs(d2))? h3x/step:h3/step;
      if ((d1!=0)||(d2!=0)) i8.set(x-lon1, lat2-y-1, 0);

    }
  }

  image_png::save(i7, "srtm_interp7.png"); // bilinear
  image_png::save(i8, "srtm_interp8.png"); // bicubic


}
