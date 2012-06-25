//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include "loaders/image_png.h"
#include "loaders/image_tiff.h"

int main(){

  iImage a = image_png::load("test.png");

//  image_tiff::save(a, iRect(-100,-100,200,200),"tst1.tif");
  image_png::save(a,"tst1.png");

}


