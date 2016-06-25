#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* 2d/rainbow example */

int
main(){

  struct rainbow_data RD[]={ // user-defined rainbow data
    {0.1, 0x000000}, // 0.1 - 0.5 black -> blue
    {0.5, 0xFF0000}, // - color step
    {0.5, 0xFF00FF}, // 0.5 - 0.9 magenta -> black
    {0.9, 0x000000},
  };
  int RDS = sizeof(RD)/sizeof(rainbow_data);

  simple_rainbow SR(0.1, 0.9);

  iImage img(256,256);

  for (int x=0; x<img.w; x++){

    double val = (double)x/(double)img.w;
    // user-defined rainbow
    int c1=get_rainbow(val, RD, RDS);
    // simple rainbow
    int c2=SR.get(val);
    // simple rainbow, black/white colors outside the min-max range
    int c3=SR.get_bnd(val,0,0xffffff);

    int y;
    for (y=0;   y<40;  y++) img.set(x,y,c1);
    for (y=50;  y<90;  y++) img.set(x,y,c2);
    for (y=100; y<140; y++) img.set(x,y,c3);
  }
  image_png::save(img, "rainbow.png");
}
