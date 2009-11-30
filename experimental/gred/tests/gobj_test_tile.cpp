#include <limits.h>
#include "gobj_test_tile.h"

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
  return iRect(
    iPoint(INT_MIN/2, INT_MIN/2),
    iPoint(INT_MAX/2, INT_MAX/2));
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
