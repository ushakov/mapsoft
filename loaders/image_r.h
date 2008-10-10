#ifndef IMAGE_R_H
#define IMAGE_R_H

// загрузка/сохранение произольного растрового изображения
// (jpeg, tiff, png)

#include "../lib2d/image.h"
#include "../utils/options.h"

// из-за фигни с setjmp.h надо, чтобы image_png.h был раньше
// чем image_jpeg.h
#include "image_png.h"
#include "image_jpeg.h"
#include "image_tiff.h"

#include <string.h>

namespace image_r{

Point<int> size(const char *file);

// loading from Rect in file to Rect in image
int load(const char *file, Rect<int> src_rect, 
         Image<int> & image, Rect<int> dst_rect);

// load the whole image 
Image<int> load(const char *file, const int scale=1);

// save the whole image
// options:
//   jpeg_quality  0..100, default 75
//   tiff_usealpha 0..1,   default 0

int save(const Image<int> & im, const Rect<int> & src_rect,
         const char *file, const Options & opts);

// save the whole image
int save(const Image<int> & im, const char * file, const Options & opts);

} // namespace
#endif
