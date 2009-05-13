#ifndef GPLANE_H
#define GPLANE_H

#include "../lib2d/rect.h"
#include "../lib2d/image.h"

typedef unsigned int GCoord;
const GCoord GCoord_min=0;
const GCoord GCoord_max=UINT_MAX;
const GCoord GCoord_cnt=UINT_MAX/2;

class GPlane{
  public:
  virtual Image<int> draw(const Rect<GCoord> &range) = 0;
};


class GPlane_test1: public GPlane{
  Image<int> draw(const Rect<GCoord> &range){
    std::cerr << "GPlane_test1: " << range << "\n";

    Image<int> ret(range.w, range.h, 0xFFFFFFFF);

    for (int j=0; j<range.h; j++){
      for (int i=0; i<range.w; i++){
        ret.set(i,j, 
          (0xFF << 24) + ((i*256)/range.w << 16) + ((j*256)/range.h << 8));
      }
    }
    return ret;
  }
};

class GPlane_test2: public GPlane{
  Image<int> draw(const Rect<GCoord> &range){
    std::cerr << "GPlane_test1: " << range << "\n";

    Image<int> ret(range.w, range.h, 0xFFFFFFFF);

    for (int j=0; j<range.h; j++){
      for (int i=0; i<range.w; i++){
        ret.set(i,j,
          (0xFF << 24) + ((i*256)/range.w << 8) + (j*256)/range.h);
      }
    }
    usleep(100000);
    return ret;
  }
};


#endif
