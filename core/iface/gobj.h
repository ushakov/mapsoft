#ifndef GOBJ_IFACE_H
#define GOBJ_IFACE_H

#include "lib2d/rect.h"
#include "lib2d/image.h"
#include <limits.h>

// return codes for draw function
const int GOBJ_FILL_NONE = 0; // object draws nothing
const int GOBJ_FILL_PART = 1; // object draws some points
const int GOBJ_FILL_ALL  = 2; // object fills in the whole image with opaque colors

const iRect GOBJ_MAX_RANGE(
  iPoint(INT_MIN/2, INT_MIN/2),
  iPoint(INT_MAX/2, INT_MAX/2));

class GObj{
public:
  virtual int draw(iImage &img, const iPoint &origin) = 0;
  virtual iRect range(void){ return GOBJ_MAX_RANGE; }

  // object can be drawn in "gray mode":
  // gray colors, borders instead of images etc.
  bool is_gray(){return gray_mode;}
  void gray_on(){gray_mode=true;}
  void gray_off(){gray_mode=false;}

private:
  bool gray_mode;
};

#endif
