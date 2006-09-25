//#include "pointset.h"
#include <iostream>
#include "../../utils/image.h"
#include "../../utils/image_io_tiff.h"
#include "../../utils/image_io_jpeg.h"

main(){
//  Image<int> i = tiff_image::load("im4.tif", Rect<int>(0,0,180,130), 8);
//  Image<int> i = jpeg_image::load("/d2/1km-kar/1-Prjazha_Svjatozero.jpg", 8);

  Point<int> i = tiff_image::size("im_5.jpg");

//  tiff_image::load_to_image("im_5.tif", Rect<int>(0,0,0,0), i, Rect<int>(50,550,550,50));
//  std::cerr << i << "\n";
//  jpeg_image::save("im_5.jpg", i, 50);
}


