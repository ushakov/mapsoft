#include <limits.h>
#include "gobj_test_grid.h"

GObjTestGrid::GObjTestGrid(const int delay_):delay(delay_){}

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
  if (delay) usleep(delay);
  return ret;
}

iRect
GObjTestGrid::range(void){
  return iRect(
    iPoint(INT_MIN/2, INT_MIN/2),
    iPoint(INT_MAX/2, INT_MAX/2));
}
