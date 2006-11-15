//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <image_png.h>
#include <image_tiff.h>

int main(){

//  std::cerr << image_png::size("google_test.png");
  Image<int> a = image_png::load("google_test.png");
//  std::cerr << a.range() << "\n";

//  image_tiff::save(a, Rect<int>(-100,-100,200,200),"tst1.tif");
  image_tiff::save(a,"tst1.tif");

}


