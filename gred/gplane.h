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
    for (int i=0; i<range.w; i++){
      ret.set(i,0,         0xFF00FF00);
      ret.set(i,range.h-1, 0xFF0000FF);
      ret.set(i,(i*range.h)/range.w,0xFF00FF00);
      ret.set(i,((range.w-i-1)*range.h)/range.w,0xFF0000FF);
    }
    for (int j=0; j<range.h; j++){
      ret.set(0, j, 0xFF00FF00);
      ret.set(range.w-1, j, 0xFF0000FF);
    }
    return ret;
  }
};


#endif
