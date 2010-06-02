#include <limits.h>
#include "gobj_solid_fill.h"

GObjSolidFill::GObjSolidFill(const int c): color(c) {
}

iImage
GObjSolidFill::draw(const iRect &range){
  return iImage(range.w, range.h, color);
}

iRect
GObjSolidFill::range(void){
  return iRect(
    iPoint(INT_MIN/2, INT_MIN/2),
    iPoint(INT_MAX/2, INT_MAX/2));
}
