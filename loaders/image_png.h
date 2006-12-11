#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "../utils/image.h"
#include "../utils/rect.h"
#include "../utils/point.h"

// lippng ��������� setjmp.h,
// � ����� ��� �� ����� ��������� image_jpeg...
#define SETJMP
#include <png.h>

namespace image_png{

// getting file dimensions
Point<int> size(const char *file);

// loading from Rect in file to Rect in image
int load(const char *file, Rect<int> src_rect, 
         Image<int> & image, Rect<int> dst_rect);

/*
// save part of image
int save(const Image<int> & im, const Rect<int> & src_rect, 
         const char *file, int quality=75)
*/

// load the whole image -- �� ������� �� �������, ��������, ���� ��������� � image_io.h
Image<int> load(const char *file, const int scale=1);

/*
// save the whole image
int save(const Image<int> & im, const char * file, int quality=75);
*/
} // namespace
#endif
