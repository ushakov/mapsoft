#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <string>
#include "image.h"
#include "rect.h"

Point<int> image_size(std::string file);

Image<RGB> image_load_RGB(std::string file, Rect<int> R, int scale = 1);
Image<RGB> image_load_RGB(std::string file, int scale = 1);

Image<RGBA> image_load_RGBA(std::string file, Rect<int> R, int scale = 1);
Image<RGBA> image_load_RGBA(std::string file, int scale = 1);

int image_save_window(std::string file, Image<RGB> im);
int image_save(std::string file, Image<RGB> im);

int image_save_window(std::string file, Image<RGBA> im);
int image_save(std::string file, Image<RGBA> im);

#endif
