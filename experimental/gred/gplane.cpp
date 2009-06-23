#include <limits.h>
#include "gplane.h"

const int GCoordMin=INT_MIN/2;
const int GCoordMax=INT_MAX/2;

Image<int> GPlaneTestTile::draw(const Rect<int> &range) const {

  Image<int> ret(range.w, range.h, 0xFFFFFFFF);
  for (int j=0; j<range.h; j++){
    for (int i=0; i<range.w; i++){
      ret.set(i,j, (0xFF << 24) + ((i*256)/range.w << 16) + ((j*256)/range.h << 8));
    }
  }
  return ret;
}

Image<int> GPlaneTestTileSlow::draw(const Rect<int> &range) const {
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

Image<int> GPlaneSolidFill::draw(const Rect<int> &range) const {
  return Image<int>(range.w, range.h, color);
}

Image<int> GPlaneTestGrid::draw(const Rect<int> &range) const {
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

Image<int> GPlaneTestGridSlow::draw(const Rect<int> &range) const {
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


