#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "2d/image.h"

namespace image_jpeg {

// getting file dimensions
iPoint size(const char *file);

// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
iImage load(const char *file, const int scale=1);

// save the whole image
int save(const iImage & im, const char * file, int quality=75);

} // namespace
#endif
