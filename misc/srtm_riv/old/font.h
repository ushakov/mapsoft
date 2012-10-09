#ifndef SRTM_FONT_H
#define SRTM_FONT_H

#define FONT_4x6D 1

#include <set>
#include <2d/point.h>

struct font{
  int width;
  int height;
  const char *letters;
  const char *images;
  font(int fontid);
  std::set<iPoint> print(int lat, int lon, const char * text);
};

#endif
