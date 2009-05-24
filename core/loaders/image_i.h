#ifndef IMAGE_I_H
#define IMAGE_I_H

#include "../lib2d/image.h"

namespace image_i{

// loading from Rect in image to Rect in image
int load(Image<int> & src_img, Rect<int> src_rect, 
         Image<int> & dst_img, Rect<int> dst_rect);

} // namespace
#endif
