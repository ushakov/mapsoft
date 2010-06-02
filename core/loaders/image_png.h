#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "lib2d/image.h"

namespace image_png{

// getting file dimensions
iPoint size(const char *file);

// loading from Rect in file to Rect in image
int load(const char *file, iRect src_rect, 
         iImage & image, iRect dst_rect);


// save part of image
int save(const iImage & im, const iRect & src_rect, 
         const char *file);


// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
iImage load(const char *file, const int scale=1);


// save the whole image
int save(const iImage & im, const char * file);

} // namespace
#endif
