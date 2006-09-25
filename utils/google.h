#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "point.h"
#include "rect.h"
#include "cache.h"
#include "image.h"
#include "image_io_jpeg.h"

namespace google{

const int google_scale_min = 1;
const int google_scale_max = 18;

const char lett[4] = {'q','r','t','s'};

// пересчет координат кусочка в его адрес
// поменять организацию локальных данных!
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

// пересчет геодезических координат в пикселы
Point<int> lonlat2xy(int google_scale, Point<double> lonlat){
    double width = pow(2, google_scale-1);
    lonlat.y = 180.0/M_PI * log(tan(M_PI/180.0*(45.0+lonlat.y/2.0)));
    int X = int((lonlat.x+180) * 256.0 / 360.0 * width);
    int Y = int((180-lonlat.y) * 256.0 / 360.0 * width);
    return Point<int>(X,Y);
}

// то же для прямоугольников
Rect<int> lonlat2xy(int google_scale, Rect<double> lonlat){
    return Rect<int>(lonlat2xy(google_scale, lonlat.TLC),lonlat2xy(google_scale, lonlat.BRC));
}

/*// Загрузка картинки
Image<int> load_image(const std::string & dir, int google_scale, Rect<int> window, int scale){
 
  int image_width = 256*(int)pow(2,google_scale-1);

  Rect<int> google_points = Rect<int>(0,0,image_width,image_width);
  Rect<int> google_tiles  = google_points/256;

  clip_rect_to_rect(window, google_points);
  Rect<int> window_tiles  = window/256;

  if (google_scale<google_scale_min) google_scale=google_scale_min;
  if (google_scale>google_scale_max) google_scale=google_scale_max;

  Image<int> ret(window.width()/scale, window.height()/scale);

  for (int yt = window_tiles.TLC.y; yt<=window_tiles.BRC.y; yt++){
    for (int xt = window_tiles.TLC.x; xt<=window_tiles.BRC.x; xt++){
      int x1 = 0;   if (xt==window_tiles.TLC.x) x1 = window.TLC.x % 256;
      int x2 = 256; if (xt==window_tiles.BRC.x) x2 = window.BRC.x % 256;
      int y1 = 0;   if (yt==window_tiles.TLC.y) y1 = window.TLC.y % 256;
      int y2 = 256; if (yt==window_tiles.BRC.y) y2 = window.BRC.y % 256;
      if ((x1==x2) || (y1==y2)) continue;
      std::string addr = dir + tile2file(google_scale, xt,yt);
      Rect<int> src_rect(x1,y1,x2,y2);
      Rect<int> dst_rect((x1 + xt*256 - window.TLC.x)/scale,
                         (y1 + yt*256 - window.TLC.y)/scale,
                         (x2 + xt*256 - window.TLC.x)/scale,
                         (y2 + yt*256 - window.TLC.y)/scale);

//      std::cerr << addr << " " << src_rect << " --> " << dst_rect << "\n";
      
      jpeg_image::load_to_image(addr.c_str(), src_rect, ret, dst_rect);
    }
  }
  return ret;
} */


// Загрузка картинки
int load_to_image(const std::string & dir, int google_scale, Rect<int> src_rect, Image<int> & image, Rect<int> dst_rect){

  if (google_scale<google_scale_min) google_scale=google_scale_min;
  if (google_scale>google_scale_max) google_scale=google_scale_max;
 
  int src_width = 256*(int)pow(2,google_scale-1);

  Rect<int> src_points = Rect<int>(0,0,src_width,src_width);

  clip_rect_to_rect(src_rect, src_points);
  clip_rect_to_rect(dst_rect, Rect<int>(0,0,image.w, image.h));

  Rect<int> src_tiles  = src_rect/256;

  for (int yt = src_tiles.TLC.y; yt<=src_tiles.BRC.y; yt++){
    for (int xt = src_tiles.TLC.x; xt<=src_tiles.BRC.x; xt++){

      int sx1 = 0;   if (xt==src_tiles.TLC.x) sx1 = src_rect.TLC.x % 256;
      int sy1 = 0;   if (yt==src_tiles.TLC.y) sy1 = src_rect.TLC.y % 256;
      int sx2 = 256; if (xt==src_tiles.BRC.x) sx2 = src_rect.BRC.x % 256;
      int sy2 = 256; if (yt==src_tiles.BRC.y) sy2 = src_rect.BRC.y % 256;
      if ((sx1==sx2) || (sy1==sy2)) continue;

      int dx1 = ((256*xt + sx1 - src_rect.TLC.x) * dst_rect.width())/src_rect.width() + dst_rect.TLC.x;
      int dx2 = ((256*xt + sx2 - src_rect.TLC.x) * dst_rect.width())/src_rect.width() + dst_rect.TLC.x;
      int dy1 = ((256*yt + sy1 - src_rect.TLC.y) * dst_rect.height())/src_rect.height() + dst_rect.TLC.y;
      int dy2 = ((256*yt + sy2 - src_rect.TLC.y) * dst_rect.height())/src_rect.height() + dst_rect.TLC.y;

      std::string addr = dir + tile2file(google_scale, xt,yt);

      Rect<int> src(sx1,sy1,sx2,sy2);
      Rect<int> dst(dx1,dy1,dx2,dy2);
      std::cerr << addr << " " << src << " --> " << dst << "\n";
      
      jpeg_image::load_to_image(addr.c_str(), src, image, dst);
    }
  }
  return 0;
}

Image<int> load(const std::string & dir, int google_scale, const Rect<int> & src_rect, int scale=1){
  int w = src_rect.width()/scale;
  int h = src_rect.height()/scale;
  Rect<int> dst_rect(0,0,w,h);
  Image<int> ret(w,h);
  load_to_image(dir, google_scale, src_rect, ret, dst_rect);
  return ret;
}

// то же, для координат wgs84
int load_to_image(const std::string & dir, int google_scale, const Rect<double> & src_rect, 
                  Image<int> & image, const Rect<int> & dst_rect){
  return load_to_image(dir, google_scale, lonlat2xy(google_scale, src_rect), image, dst_rect);
}

Image<int> load(const std::string & dir, int google_scale, const Rect<double> & src_rect, int scale=1){
    return load(dir, google_scale, lonlat2xy(google_scale, src_rect), scale);
}

// случайный доступ с кэшированием картинок
struct points{

  std::string dir;
  int google_scale;

  int width;  // width = 2**(google_scale-1), размер полной картинки: 256*width х 256*width

  Cache <Point<int>, Image<int> > cache;

  points(std::string _dir, int _google_scale, int cache_size=200): cache(cache_size){
    if (google_scale<google_scale_min) google_scale=google_scale_min;
    if (google_scale>google_scale_max) google_scale=google_scale_max;
    google_scale=_google_scale;
    width=int(pow(2,google_scale-1));
    dir=_dir;
  }

  int get(Point<int> p){
    // x,y -- 0 .. 256*2**(google_scale-1)-1
    if  ((p.x>=256*width)|| 
         (p.y>=256*width)|| 
         (p.x<0)||(p.y<0)) return 0;

    int x = p.x/256;  // 0 .. 2**(google_scale-1)-1
    int y = p.y/256;
    int dx = p.x%256; // 0 .. 255
    int dy = p.y%256;

    Point<int> key_pair(x,y);

    if (!cache.contains(key_pair)){
      std::cerr << "ADD " << key_pair << "\n";
      std::string addr = dir + tile2file(google_scale,x,y);
      Image<int> im = jpeg_image::load(addr.c_str());
      if (im.empty()) im = Image<int>(256,256,0);
      cache.add(key_pair, im);
    }

    return cache.get(key_pair).get(dx,dy);
  }

  int get(Point<double> p){
    return get(lonlat2xy(google_scale, p));
  }

};

}//namespace
