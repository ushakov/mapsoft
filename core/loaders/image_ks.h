#ifndef IMAGE_KS_H
#define IMAGE_KS_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <curl/curl.h>
#include <sys/stat.h>

#include "2d/cache.h"
#include "2d/image.h"
#include "image_jpeg.h"

namespace ks{

const int ks_scale_min = 3;
const int ks_scale_max = 17;

// Загрузка картинки
int load(
    const std::string & dir, 
    int ks_scale,	
    iRect src_rect, 
    iImage & image, 
    iRect dst_rect,
    bool do_download = false);

iImage load(
    const std::string & dir,
    int ks_scale,
    const iRect & src_rect,
    int scale=1,
    bool do_download = false);

}//namespace
#endif
