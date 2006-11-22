#ifndef IMAGE_GOOGLE_MISC_H
#define IMAGE_GOOGLE_MISC_H

//Это то, что не уложилось в концепцию image loader'a и в конце-концов должно исчезнуть...
// 

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "point.h"
#include "rect.h"
#include "cache.h"
#include "image.h"
#include "image_jpeg.h"
#include "image_google.h"

namespace google{

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
    Point<int> p1 = lonlat2xy(google_scale, lonlat.TLC());
    Point<int> p2 = lonlat2xy(google_scale, lonlat.BRC());

    if (p2.x < p1.x) std::swap(p2.x, p1.x);
    if (p2.y < p1.y) std::swap(p2.y, p1.y);
    return Rect<int>(p1,p2);
}

// загрузка для координат wgs84
int load(const std::string & dir, int google_scale, const Rect<double> & src_rect,
                  Image<int> & image, const Rect<int> & dst_rect){
  return load(dir, google_scale, lonlat2xy(google_scale, src_rect), image, dst_rect);
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
      std::string addr = dir + tile2file(google_scale,x,y);
#ifdef DEBUG_GOOGLE
      std::cerr << "google: loading " << key_pair << " (" << addr << ")\n";
#endif      
      Image<int> im = image_jpeg::load(addr.c_str());
      if (im.empty()) im = Image<int>(256,256,0);
      cache.add(key_pair, im);
    }

    return cache.get(key_pair).get(dx,dy);
  }

  int get(Point<double> p){
    return get(lonlat2xy(google_scale, p));
  }

  
};

  // wget -O $dir/$1.jpg "http://kh$n.google.com/kh?n=404&v=9&t=$1"

}//namespace

#endif
