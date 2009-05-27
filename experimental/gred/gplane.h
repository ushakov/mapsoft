#ifndef GPLANE_H
#define GPLANE_H

#include <limits.h>

#include "../../core/lib2d/rect.h"
#include "../../core/lib2d/image.h"

typedef unsigned int GCoord;
const GCoord GCoord_min=0;
const GCoord GCoord_max=UINT_MAX;
const GCoord GCoord_cnt=UINT_MAX/2;

Point<int> get_shift(const Point<GCoord> & p1, const Point<GCoord> & p2){ 
  return Point<int>(p1.x>p2.x? p1.x-p2.x:-(p2.x-p1.x), 
                    p1.y>p2.y? p1.y-p2.y:-(p2.y-p1.y)); 
} 


class GPlane{
  public:
  virtual Image<int> draw(const Rect<GCoord> &range) const = 0;
};


class GPlaneTestTile: public GPlane{
  Image<int> draw(const Rect<GCoord> &range) const {
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

class GPlaneTestTileSlow: public GPlane{
  Image<int> draw(const Rect<GCoord> &range) const {
    Image<int> ret(range.w, range.h, 0xFFFFFFFF);

    for (int j=0; j<range.h; j++){
      for (int i=0; i<range.w; i++){
        ret.set(i,j,
          (0xFF << 24) + (255-(i*256)/range.w << 8) + (j*256)/range.h);
      }
    }
    usleep(range.w*range.h*10);
    return ret;
  }
};

class GPlaneSolidFill: public GPlane{
  int color;
  public:
  GPlaneSolidFill(int c=0xFF000000): color(c) {}

  Image<int> draw(const Rect<GCoord> &range) const {
    return Image<int>(range.w, range.h, color);
  }
};

class GPlaneTestGrid: public GPlane{
  Image<int> draw(const Rect<GCoord> &range) const {
    Image<int> ret(range.w, range.h,0xFF000000);
    for (int j=0; j<range.h; j++){
      for (int i=0; i<range.w; i++){
        int x=range.x+i, y=range.y+j;
        for (int n=256; n>1; n/=2){
          if ((x%n==0) || (y%n==0)){
            n--;
            ret.set(i,j, (0xFF<<24) + (n<<16) + (n<<8) + n);
            break;
          }
        }
      }
    }
    return ret;
  }
};

class GPlaneTestGridSlow: public GPlane{
  Image<int> draw(const Rect<GCoord> &range) const {
    Image<int> ret(range.w, range.h, 0xFF000000);
    for (int j=0; j<range.h; j++){
      for (int i=0; i<range.w; i++){
        int x=range.x+i, y=range.y+j;
        for (int n=256; n>1; n/=2){
          if ((x%n==0) || (y%n==0)){
            n--;
            ret.set(i,j, (0xFF<<24) + (n<<16) + (n<<8) + n);
            break;
          }
        }
      }
    }
    usleep(150000);
    return ret;
  }
};


#endif
