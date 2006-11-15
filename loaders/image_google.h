#ifndef IMAGE_GOOGLE_H
#define IMAGE_GOOGLE_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "../utils/point.h"
#include "../utils/rect.h"
#include "../utils/cache.h"
#include "../utils/image.h"
#include "image_jpeg.h"

namespace google{

const int google_scale_min = 1;
const int google_scale_max = 18;

const char lett[4] = {'q','r','t','s'};

// пересчет координат кусочка в его адрес
std::string tile2addr(int google_scale, int xt, int yt){

  std::string addr="t";
  int h = (int)pow(2,google_scale-1);
  for (int i=google_scale-1; i>0; i--){
    char a=0;	
    h /= 2; // half of picture width
    if (xt>=h) {a|=1; xt-=h;}
    if (yt>=h) {a|=2; yt-=h;}
    addr += lett[a];
  }
  return addr;
}

std::string tile2file(int google_scale, int xt, int yt){
  std::ostringstream addr;
  addr << "/" << std::setfill('0') << std::setw(2) << google_scale 
       << "/" << tile2addr(google_scale, xt, yt) << ".jpg";
  return addr.str();
}

// Загрузка картинки
int load(
    const std::string & dir, 
    int google_scale,	
    Rect<int> src_rect, 
    Image<int> & image, 
    Rect<int> dst_rect)
{

  if (google_scale<google_scale_min) google_scale=google_scale_min;
  if (google_scale>google_scale_max) google_scale=google_scale_max;
 
  int src_width = 256*(int)pow(2,google_scale-1);

  Rect<int> src_points = Rect<int>(0,0,src_width,src_width);

  // подрежем прямоугольники
  clip_rects_for_image_loader(
      src_points, src_rect,
      Rect<int>(0,0,image.w,image.h), dst_rect);

  if (src_rect.empty() || dst_rect.empty()) return 0;


  Rect<int> src_tiles  = rect_intdiv(src_rect,256);

  for (int yt = src_tiles.TLC().y; yt<src_tiles.BRC().y; yt++){
    for (int xt = src_tiles.TLC().x; xt<src_tiles.BRC().x; xt++){

      int sx1 = 0;   if (xt==src_tiles.TLC().x) sx1 = src_rect.TLC().x % 256;
      int sy1 = 0;   if (yt==src_tiles.TLC().y) sy1 = src_rect.TLC().y % 256;
      int sx2 = 256; if (xt==src_tiles.BRC().x-1) sx2 = (src_rect.BRC().x-1) % 256 + 1;
      int sy2 = 256; if (yt==src_tiles.BRC().y-1) sy2 = (src_rect.BRC().y-1) % 256 + 1;
      if ((sx1==sx2) || (sy1==sy2)) continue;

      int dx1 = ((256*xt + sx1 - src_rect.x) * dst_rect.w)/src_rect.w + dst_rect.x;
      int dx2 = ((256*xt + sx2 - src_rect.x) * dst_rect.w)/src_rect.w + dst_rect.x;
      int dy1 = ((256*yt + sy1 - src_rect.y) * dst_rect.h)/src_rect.h + dst_rect.y;
      int dy2 = ((256*yt + sy2 - src_rect.y) * dst_rect.h)/src_rect.h + dst_rect.y;

      std::string addr = dir + tile2file(google_scale, xt,yt);

      Rect<int> src(sx1,sy1,sx2-sx1,sy2-sy1);
      Rect<int> dst(dx1,dy1,dx2-dx1,dy2-dy1);
#ifdef DEBUG_GOOGLE
      std::cerr << "google: loading " << addr << " " << src << " --> " << dst << "\n";
#endif      
      image_jpeg::load(addr.c_str(), src, image, dst);
    }
  }
  return 0;
}

Image<int> load(const std::string & dir, int google_scale, const Rect<int> & src_rect, int scale=1){
  int w = src_rect.w/scale;
  int h = src_rect.h/scale;
  Rect<int> dst_rect(0,0,w,h);
  Image<int> ret(w,h);
  load(dir, google_scale, src_rect, ret, dst_rect);
  return ret;
}

}//namespace
#endif
