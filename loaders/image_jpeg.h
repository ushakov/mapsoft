#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "../utils/image.h"
#include "../utils/rect.h"
#include "../utils/point.h"

#include <jpeglib.h>

#ifndef SETJMP
#define SETJMP
#include <setjmp.h>
#endif

namespace image_jpeg{

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

void my_error_exit (j_common_ptr cinfo);


// getting file dimensions
Point<int> size(const char *file);

// loading from Rect in jpeg-file to Rect in image
int load(const char *file, Rect<int> src_rect, 
         Image<int> & image, Rect<int> dst_rect);

// save part of image
int save(const Image<int> & im, const Rect<int> & src_rect, 
         const char *file, int quality=75);

// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
Image<int> load(const char *file, const int scale=1);

// save the whole image
int save(const Image<int> & im, const char * file, int quality=75);

} // namespace
#endif
