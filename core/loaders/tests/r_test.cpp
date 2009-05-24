//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <image_r.h>

int main(){

//  std::cerr << image_r::size("google_test.png");
  Image<int> a = image_r::load("google_test.png");
//  std::cerr << a.range() << "\n";

//  image_tiff::save(a, Rect<int>(-100,-100,200,200),"tst1.tif");
// int32 b;

  Options opt;
  image_r::save(a,"tst1.tif",opt);

}


