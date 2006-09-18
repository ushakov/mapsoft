#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include "image.h"
#include "rect.h"

Point<int> image_size(std::string file);

Image<RGB> load_image(std::string file, Rect<int> R, int scale = 1){
}
Image<RGB> load_image(std::string file, int scale = 1){
}

Image<RGBA> load_image_with_alpha(std::string file, Rect<int> R, int scale = 1){
}
Image<RGBA> load_image_with_alpha(std::string file, int scale = 1){
}

int save_image(std::string file, Image<RGB> im, Rect<int> R){
}
int save_image(std::string file, Image<RGB> im){
}
int save_image(std::string file, Image<RGBA> im, Rect<int> R){
}
int save_image(std::string file, Image<RGBA> im){
}

#endif
