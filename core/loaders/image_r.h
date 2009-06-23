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

iPoint size(const char *file);

// loading from Rect in file to Rect in image
int load(const char *file, iRect src_rect, 
         iImage & image, iRect dst_rect);

// load the whole image 
iImage load(const char *file, const int scale=1);

// save the whole image
// options:
//   jpeg_quality  0..100, default 75
//   tiff_usealpha 0..1,   default 0

int save(const iImage & im, const iRect & src_rect,
         const char *file, const Options & opts);

// save the whole image
int save(const iImage & im, const char * file, const Options & opts);

} // namespace
#endif
