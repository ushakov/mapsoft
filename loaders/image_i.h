#ifndef IMAGE_I_H
#define IMAGE_I_H

#include "../utils/image.h"
#include "../utils/rect.h"
#include "../utils/point.h"

namespace image_i{

// loading from Rect in image to Rect in image
int load(Image<int> & src_img, Rect<int> src_rect, 
         Image<int> & dst_img, Rect<int> dst_rect){

    // подрежем прямоугольники
    clip_rects_for_image_loader(
      src_img.range(), src_rect, dst_img.range(), dst_rect);
    if (src_rect.empty() || dst_rect.empty()) return 1;
    
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      int src_y = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y == src_rect.BRC().y) src_y--;

      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        int src_x = src_rect.x + ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
        if (src_x == src_rect.BRC().x) src_x--;
        dst_img.set(dst_x, dst_y, src_img.get(src_x,src_y));
      }
    }
    return 0;
}
} // namespace
#endif
