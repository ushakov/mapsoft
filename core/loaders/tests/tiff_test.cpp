#define DEBUG_TIFF
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <image_tiff.h>
#include <image_jpeg.h>

int main(){

  iImage a = image_tiff::load("tst1.tif");
  std::cerr << a.range() << "\n";

//  image_tiff::save(a, iRect(-100,-100,200,200),"tst2.tif");
  image_tiff::save(a,"tst2.tif");

}


