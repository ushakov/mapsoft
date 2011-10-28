#ifndef SRTM_FONT_H
#define SRTM_FONT_H

#include "point.h"

#define FONT_4x6D 1

struct font{
  int width;
  int height;
  char *letters;
  char *images;
  font(int fontid);
  std::set<point> print(int lat, int lon, const char * text);
};

#endif
