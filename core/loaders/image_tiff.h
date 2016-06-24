#ifndef IMAGE_TIFF_H
#define IMAGE_TIFF_H

#include "2d/image.h"

namespace image_tiff{

// getting file dimensions
iPoint size(const char *file);

iImage load(const char *file, const int scale=1);

// save the whole image
int save(const iImage & im, const char * file);

} // namespace
#endif
