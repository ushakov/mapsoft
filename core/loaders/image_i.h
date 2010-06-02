#ifndef IMAGE_I_H
#define IMAGE_I_H

#include "lib2d/image.h"

namespace image_i{

// loading from Rect in image to Rect in image
int load(iImage & src_img, iRect src_rect, 
         iImage & dst_img, iRect dst_rect);

} // namespace
#endif
