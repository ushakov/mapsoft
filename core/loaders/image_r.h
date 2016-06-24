#ifndef IMAGE_R_H
#define IMAGE_R_H

// загрузка/сохранение произольного растрового изображения
// (jpeg, tiff, png)

#include "2d/image.h"
#include "options/options.h"

// из-за фигни с setjmp.h надо, чтобы image_png.h был раньше
// чем image_jpeg.h
#include "image_png.h"
#include "image_jpeg.h"
#include "image_tiff.h"
#include "image_gif.h"

namespace image_r{

iPoint size(const char *file);

// load the whole image 
iImage load(const char *file, const int scale=1);

// save the whole image
int save(const iImage & im, const char * file, const Options & opts = Options());

} // namespace
#endif
