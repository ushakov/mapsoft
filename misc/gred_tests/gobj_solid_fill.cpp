#include "gobj_solid_fill.h"

GObjSolidFill::GObjSolidFill(const int c): color(c) {
}

int
GObjSolidFill::draw(iImage &img, const iPoint &origin){
  if (color>>24 == 0){
    return GObj::FILL_NONE;
  }
  if (color>>24 == 0xFF){
    img.fill(color);
    return GObj::FILL_ALL;
  }
  img.fill_a(color);
  return GObj::FILL_PART;
}

