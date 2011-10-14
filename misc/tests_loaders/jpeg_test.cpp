//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include "loaders/image_jpeg.h"

int main(){

  iImage a = image_jpeg::load("google_test1.jpg");
  std::cerr << a.range() << "\n";

  image_jpeg::save(a, iRect(-100,-100,200,200),"tst1.jpg");
//  image_jpeg::save(a,"tst1.jpg");

}


