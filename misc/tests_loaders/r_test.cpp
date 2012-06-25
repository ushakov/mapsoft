//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include "loaders/image_r.h"

int main(){

  iImage a = image_r::load("test.png");

//  image_tiff::save(a, iRect(-100,-100,200,200),"tst1.tif");
// int32 b;

  Options opt;
  image_r::save(a,"tst1.tif",opt);

}


