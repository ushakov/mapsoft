#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "../lib2d/image.h"

// lippng загружает setjmp.h,
// а потом его же может загружать image_jpeg...
#define SETJMP
#include <png.h>

namespace image_png{

// getting file dimensions
Point<int> size(const char *file);

// loading from Rect in file to Rect in image
int load(const char *file, Rect<int> src_rect, 
         Image<int> & image, Rect<int> dst_rect);


// save part of image
int save(const Image<int> & im, const Rect<int> & src_rect, 
         const char *file);


// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
Image<int> load(const char *file, const int scale=1);


// save the whole image
int save(const Image<int> & im, const char * file);

} // namespace
#endif
