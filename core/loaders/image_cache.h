#ifndef IMAGE_CACHE
#define IMAGE_CACHE

#include <2d/image.h>
#include <2d/line.h>
#include <cache/cache.h>
#include <string>
#include <loaders/image_r.h>

// class for image cache
class ImageCache {
  Cache<std::string, std::pair<int, iImage> > cache;

public:
  ImageCache(const int maxnum) : cache(maxnum){}

  // Load image with scale sc or use already loaded image.
  // Border is applied only after loading.
  iImage get(const std::string & fn,
             const int sc = 1,
             const iLine & border = iLine()){
    if (cache.contains(fn)){
      std::pair<int, iImage> ip = cache.get(fn);
      if (ip.first == sc) return ip.second;
    }
    iImage img = image_r::load(fn.c_str(), sc);
    if (border.size()) img.set_border(border/sc);
    cache.add(fn, std::make_pair(sc, img));
    return img;
  }
};

#endif