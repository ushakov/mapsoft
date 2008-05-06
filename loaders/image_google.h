#ifndef IMAGE_GOOGLE_H
#define IMAGE_GOOGLE_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <curl/curl.h>
#include <sys/stat.h>

#include "../lib2d/image.h"
#include "../utils/cache.h"
#include "image_jpeg.h"

namespace google{

const int google_scale_min = 1;
const int google_scale_max = 18;

const char lett[4] = {'q','r','t','s'};

// �������� ��������� ������� � ��� �����
// ������ ��� ������ �����!
std::string tile2addr(int google_scale, int xt, int yt);

std::string tile2file(int google_scale, int xt, int yt);

// �������� ��������
int load(
    const std::string & dir, 
    int google_scale,	
    Rect<int> src_rect, 
    Image<int> & image, 
    Rect<int> dst_rect,
    bool do_download = false);

Image<int> load(
    const std::string & dir,
    int google_scale,
    const Rect<int> & src_rect,
    int scale=1,
    bool do_download = false);

}//namespace
#endif
