//#include "pointset.h"
#include <iostream>
#include "../../utils/image.h"
#include "../../utils/image_io_tiff.h"
#include "../../utils/image_io_jpeg.h"

main(){
//  Image<int> i = tiff_image::load("im4.tif", Rect<int>(0,0,180,130), 8);
//  Image<int> i = jpeg_image::load("/d2/1km-kar/1-Prjazha_Svjatozero.jpg", 8);
  Image<int> i = jpeg_image::load("/d2/1km-kar/1-Prjazha_Svjatozero.jpg", Rect<int>(6000,10000,6100,10100), 1);
  std::cerr << i << "\n";
//  tiff_image::save("im_5.tif", i, false);
}


