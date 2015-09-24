#include "gobj_test_grid.h"

GObjTestGrid::GObjTestGrid(const int delay_):delay(delay_){}

int
GObjTestGrid::draw(iImage &img, const iPoint &origin){
  for (int j=0; j<img.h; j++){
    for (int i=0; i<img.w; i++){
      img.set(i,j,0xFF<<24);

      int x=origin.x+i, y=origin.y+j;
      for (int n=256; n>1; n/=2){
        if ((x%n==0) || (y%n==0)){
          n--;
          img.set(i,j, (0xFF<<24) + (n<<16) + (n<<8) + n);
          break;
        }
      }
    }
  }
  if (delay) usleep(delay);
  return GObj::FILL_ALL;
}

