//#include "pointset.h"
#include <iostream>
#include "../utils/image.h"
#include "../utils/color.h"


main(){

    Image<RGB> i1(100,100,RGB(1,1,1));
    Image<RGB> i2(0,0);
    i2 = i1.copy();

    std::cerr << i1.get(50,50) << "\n";
    std::cerr << i2.get(50,50) << "\n";

//  Image<RGB> i = image_tiff::load("im4.tif", Rect<int>(0,0,20,20), 2);
//  std::cerr << i;
}


