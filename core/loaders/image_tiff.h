#ifndef IMAGE_TIFF_H
#define IMAGE_TIFF_H

#include "2d/image.h"

namespace image_tiff{

// getting file dimensions
iPoint size(const char *file);

// loading from Rect in jpeg-file to Rect in image
int load(const char *file, iRect src_rect, iImage & image, iRect dst_rect);

// save part of image
int save(const iImage & im, const iRect & src_rect, const char *file);

iImage load(const char *file, const int scale=1);

// save the whole image
int save(const iImage & im, const char * file);

} // namespace
#endif
