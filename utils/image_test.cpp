//#include "pointset.h"
#include <iostream>
#include "image.h"
#include "color.h"


main(){

    Image<RGB> i1(100,100,RGB(1,1,1));
    Image<RGB> i2(i1);
    Image<RGB> i3(200,200,RGB(5,5,5));
    i3 = i2;
//    Image<RGB> i4 = i3.copy();
//    i3.set(50,50,RGB(100,100,100));

    std::cerr << i1.get(50,50) << "\n";
    std::cerr << i2.get(50,50) << "\n";
    std::cerr << i3.get(50,50) << "\n";
//    std::cerr << i4.get(50,50) << "\n";

//  Image<RGB> i = tiff_image::load("im4.tif", Rect<int>(0,0,20,20), 2);
//  std::cerr << i;
}


