#ifndef IMAGE_TIFF_H
#define IMAGE_TIFF_H

#include "../lib2d/image.h"

#include <tiffio.h>

namespace image_tiff{

// getting file dimensions
Point<int> size(const char *file);

// loading from Rect in jpeg-file to Rect in image
int load(const char *file, Rect<int> src_rect, Image<int> & image, Rect<int> dst_rect);

// save part of image
int save(const Image<int> & im, const Rect<int> & src_rect,
         const char *file, bool usealpha = false);

Image<int> load(const char *file, const int scale=1);

// save the whole image
int save(const Image<int> & im, const char * file, bool usealpha = false);

} // namespace
#endif
