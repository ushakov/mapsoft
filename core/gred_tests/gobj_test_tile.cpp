#include "gobj_test_tile.h"

GObjTestTile::GObjTestTile(const bool slow_):slow(slow_){}

int
GObjTestTile::draw(iImage &img, const iPoint &origin){
  for (int j=0; j<img.h; j++){
    for (int i=0; i<img.w; i++){
      img.set(i,j, (0xFF << 24) + ((i*256)/img.w << 16) + ((j*256)/img.h << 8));
    }
  }
  if (slow) usleep(img.w*img.h*10);
  return GOBJ_FILL_ALL;
}

