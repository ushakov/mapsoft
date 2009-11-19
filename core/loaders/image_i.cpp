#include "../lib2d/image_source.h"

namespace image_i{

// loading from Rect in image to Rect in image
int load(iImage & src_img, const iRect & src_rect,
         iImage & dst_img, const iRect & dst_rect){

    iImageSourceImage loader(src_img);
    return loader.render_to_image(dst_img, src_rect, dst_rect)? 0:1;
}
} // namespace
