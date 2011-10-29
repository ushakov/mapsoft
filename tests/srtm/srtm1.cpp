#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* srtm3 test: use geth to draw picture
*/

struct rainbow_data RD1[]={
  {   0.0, 0x000000},
  {   0.0, 0xFF0000},
  { 500.0, 0xFFFF00},
  {1000.0, 0x00FF00},
  {1500.0, 0x00FFFF},
  {2000.0, 0x0000FF},
  {2500.0, 0xFF00FF},
  {3000.0, 0xFFFFFF}
};

struct rainbow_data RD2[]={
  {1600.0, 0x000000},
  {1600.0, 0xFF0000},
  {1800.0, 0xFFFF00},
  {2000.0, 0x00FF00},
  {2200.0, 0x00FFFF},
  {2400.0, 0x0000FF},
  {2600.0, 0xFF00FF},
  {2800.0, 0xFFFFFF}
};

#define DMS(x,y,z) ((x)*3600+(y)*60+z)/3

int
main(){

  int lon1,lon2, lat1,lat2;
  srtm3 S( def_srtm_dir, 4);

  /// test1.png -- picture with tile boundaries

  lon1 = DMS(96,55,00);
  lon2 = DMS(97,05,00);
  lat1 = DMS(52,55,00);
  lat2 = DMS(53,05,00);

  iImage i1(lon2-lon1, lat2-lat1);

  for (int x=lon1; x<lon2; x++){
    for (int y=lat1; y<lat2; y++){
      i1.set(x-lon1, lat2-y-1,
        get_rainbow(S.geth(x,y),
          RD1, sizeof(RD1)/sizeof(rainbow_data)));
    }
  }
  image_png::save(i1, "test1.png");

  //// test2.png -- interpolating srtm holes

  lon1 = DMS(96,30,00);
  lon2 = DMS(96,40,00);
  lat1 = DMS(53,30,00);
  lat2 = DMS(53,40,00);

 
  iImage i2a(lon2-lon1, lat2-lat1);
  iImage i2b(lon2-lon1, lat2-lat1);

  for (int x=lon1; x<lon2; x++){
    for (int y=lat1; y<lat2; y++){
      i2a.set(x-lon1, lat2-y-1,
        get_rainbow(S.geth(x,y,true),
          RD1, sizeof(RD1)/sizeof(rainbow_data)));
      i2b.set(x-lon1, lat2-y-1,
        get_rainbow(S.geth(x,y),
          RD1, sizeof(RD1)/sizeof(rainbow_data)));
    }
  }
  image_png::save(i2a, "test2a.png");
  image_png::save(i2b, "test2b.png");

  //// test3.png -- interpolating between points

  lon1 = DMS(96,32,00)*10;
  lon2 = DMS(96,34,00)*10;
  lat1 = DMS(53,32,00)*10;
  lat2 = DMS(53,34,00)*10;

  iImage i3a(lon2-lon1, lat2-lat1);
  iImage i3b(lon2-lon1, lat2-lat1);
  iImage i3c(lon2-lon1, lat2-lat1);
  iImage i3d(lon2-lon1, lat2-lat1);
  iImage i3e(lon2-lon1, lat2-lat1);
  iImage i3f(lon2-lon1, lat2-lat1);

  for (int x=lon1; x<lon2; x++){
    for (int y=lat1; y<lat2; y++){
      short h1=S.geth(round(x/10.0), round(y/10.0));
      short h2=S.geth4(dPoint(x,y)/10.0/3600*3);
      short h3=S.geth16(dPoint(x,y)/10.0/3600*3);
      i3a.set(x-lon1, lat2-y-1,
        get_rainbow(h1, RD2, sizeof(RD2)/sizeof(rainbow_data)));
      i3b.set(x-lon1, lat2-y-1,
        get_rainbow(h2, RD2, sizeof(RD2)/sizeof(rainbow_data)));
      i3c.set(x-lon1, lat2-y-1,
        get_rainbow(h3, RD2, sizeof(RD2)/sizeof(rainbow_data)));

      h1=S.geth(round(x/10.0), round(y/10.0), true);
      h2=S.geth4(dPoint(x,y)/10.0/3600*3, true);
      h3=S.geth16(dPoint(x,y)/10.0/3600*3, true);

      i3d.set(x-lon1, lat2-y-1,
        get_rainbow(h1, RD2, sizeof(RD2)/sizeof(rainbow_data)));
      i3e.set(x-lon1, lat2-y-1,
        get_rainbow(h2, RD2, sizeof(RD2)/sizeof(rainbow_data)));
      i3f.set(x-lon1, lat2-y-1,
        get_rainbow(h3, RD2, sizeof(RD2)/sizeof(rainbow_data)));
    }
  }

  double x=lon1 + 70;
  for (double y=lat1+200; y<lat1+350; y++){
    short h;
    int y1;
    h = (S.geth(round(x/10.0), round(y/10.0), true)-1000)/10;
    i3d.set(x-lon1, lat2-y-1, 0x000000);
    i3d.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
    h = (S.geth4(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i3e.set(x-lon1, lat2-y-1, 0x000000);
    i3e.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
    h = (S.geth16(dPoint(x,y)/10.0/3600*3, true)-1000)/10;
    i3f.set(x-lon1, lat2-y-1, 0x000000);
    i3f.safe_set(y-lat1-100, lat2-lat1-h, 0x000000);
  }
  image_png::save(i3a, "test3a.png"); // nearest point
  image_png::save(i3b, "test3b.png"); // bilinear
  image_png::save(i3c, "test3c.png"); // bicubic
  image_png::save(i3d, "test3d.png");
  image_png::save(i3e, "test3e.png");
  image_png::save(i3f, "test3f.png");

  /// test4.png -- with contours

  iImage i4a(lon2-lon1, lat2-lat1);
  iImage i4b(lon2-lon1, lat2-lat1);
  iImage i4c(lon2-lon1, lat2-lat1);

  int step=30;
  for (int x=lon1; x<lon2; x++){
    int ho1=srtm_undef, ho2=srtm_undef, ho3=srtm_undef;
    for (int y=lat1; y<lat2; y++){
      short h2=S.geth4(dPoint(x,y)/10.0/3600*3, true);
      short h2x=S.geth4(dPoint(x+1,y)/10.0/3600*3, true);
      short h3=S.geth16(dPoint(x,y)/10.0/3600*3, true);
      short h3x=S.geth16(dPoint(x+1,y)/10.0/3600*3, true);

      i4b.set(x-lon1, lat2-y-1,
        get_rainbow(h2, RD2, sizeof(RD2)/sizeof(rainbow_data)));
      i4c.set(x-lon1, lat2-y-1,
        get_rainbow(h3, RD2, sizeof(RD2)/sizeof(rainbow_data)));

      int d1,d2;

      d1 = (h2/step - ho2);
      d2 = (h2x/step - ho2);
      ho2 = (abs(d1)>abs(d2))? h2x/step:h2/step;
      if ((d1!=0)||(d2!=0)) i4b.set(x-lon1, lat2-y-1, 0);

      d1 = (h3/step - ho3);
      d2 = (h3x/step - ho3);
      ho3 = (abs(d1)<abs(d2))? h3x/step:h3/step;
      if ((d1!=0)||(d2!=0)) i4c.set(x-lon1, lat2-y-1, 0);

    }
  }

  image_png::save(i4b, "test4b.png"); // bilinear
  image_png::save(i4c, "test4c.png"); // bicubic


}
