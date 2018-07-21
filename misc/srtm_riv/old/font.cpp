#include <stdexcept>
#include "font.h"

font::font(int fontid){
  switch (fontid){
  case 1:
    width=4;
    height=6;
    static const char *let = "0123456789 .";
    letters = let;
    static const char fnt[] = {
    // Здесь на одну букву больше, чем в letters. Последняя
    // подставляется вместо неизвестных букв
    1,1,1,0,
    1,0,1,0,
    1,0,1,0,
    1,0,1,0,
    1,1,1,0,
    0,0,0,0,

    0,0,1,0,
    0,1,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,0,0,

    1,1,1,0,
    0,0,1,0,
    0,0,1,0,
    0,1,0,0,
    1,1,1,0,
    0,0,0,0,

    1,1,1,0,
    0,0,1,0,
    0,1,1,0,
    0,0,1,0,
    1,1,1,0,
    0,0,0,0,

    1,0,1,0,
    1,0,1,0,
    1,1,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,0,0,

    1,1,1,0,
    1,0,0,0,
    1,1,1,0,
    0,0,1,0,
    1,1,1,0,
    0,0,0,0,

    1,1,1,0,
    1,0,0,0,
    1,1,1,0,
    1,0,1,0,
    1,1,1,0,
    0,0,0,0,

    1,1,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,0,0,

    1,1,1,0,
    1,0,1,0,
    1,1,1,0,
    1,0,1,0,
    1,1,1,0,
    0,0,0,0,

    1,1,1,0,
    1,0,1,0,
    1,1,1,0,
    0,0,1,0,
    1,1,1,0,
    0,0,0,0,

    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,

    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,

    1,1,1,0,
    1,0,1,0,
    0,0,1,0,
    0,1,0,0,
    0,0,0,0,
    0,1,0,0,
    };
    images = fnt;
    break;
  default:
    throw Err() << "Unknown font!\n";
  }
}

std::set<iPoint> font::print(int lat, int lon, const char *text){
  char *txtptr=(char *)text;
  std::set<iPoint> S;

  while (*txtptr!='\0'){
    const char *letptr=letters;
    while (*letptr!='\0'){
      if (*letptr == *txtptr) break;
      letptr++;
    }
    for (int j=0; j<height; j++){
      for (int i=0; i<width; i++){
        if (images[(letptr-letters)*width*height + j*width +i])
          S.insert(iPoint(lat + height - j, lon + (txtptr-text)*width + i));
      }
    }
    txtptr++;
  }
  return S;
}

