#include <limits.h>
#include "gobj.h"

const int GCoordMin=INT_MIN/2;
const int GCoordMax=INT_MAX/2;

iImage
GObjTestTile::draw(const iRect &range){

  iImage ret(range.w, range.h, 0xFFFFFFFF);
  for (int j=0; j<range.h; j++){
    for (int i=0; i<range.w; i++){
      ret.set(i,j, (0xFF << 24) + ((i*256)/range.w << 16) + ((j*256)/range.h << 8));
    }
  }
  return ret;
}
iRect
GObjTestTile::range(void){
  return iRect(GCoordMin,GCoordMin,GCoordMax,GCoordMax);
}


iImage
GObjTestTileSlow::draw(const iRect &range){
  iImage ret(range.w, range.h, 0xFFFFFFFF);

  for (int j=0; j<range.h; j++){
    for (int i=0; i<range.w; i++){
      ret.set(i,j,
        (0xFF << 24) + (255-(i*256)/range.w << 8) + (j*256)/range.h);
    }
  }
  usleep(range.w*range.h*10);
  return ret;
}
iRect
GObjTestTileSlow::range(void){
  return iRect(-500,-500, 4000,4000);
}


iImage
GObjSolidFill::draw(const iRect &range){
  return iImage(range.w, range.h, color);
}
iRect
GObjSolidFill::range(void){
  return iRect(GCoordMin,GCoordMin,GCoordMax,GCoordMax);
}

iImage
GObjTestGrid::draw(const iRect &range){
  iImage ret(range.w, range.h,0xFF000000);
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
iRect
GObjTestGrid::range(void){
  return iRect(GCoordMin,GCoordMin,GCoordMax,GCoordMax);
}

iImage
GObjTestGridSlow::draw(const iRect &range){
  iImage ret(range.w, range.h, 0xFF000000);
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
iRect
GObjTestGridSlow::range(void){
  return iRect(GCoordMin,GCoordMin,GCoordMax,GCoordMax);
}

