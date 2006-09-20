//#include "pointset.h"
#include <iostream>
#include "../../utils/image.h"
#include "../../utils/image_io_tiff.h"


main(){

//  Image<int> i = tiff_image::load("im4.tif", Rect<int>(0,0,180,130), 8);
  Image<int> i = tiff_image::load("im1.tif", 2);
  std::cerr << i << "\n";
  tiff_image::save("im1_.tif", i, true);
}


