#include "gobj.h"
#include <limits.h>

// return codes for draw function
const int GOBJ_FILL_NONE = 0; // object draws nothing
const int GOBJ_FILL_PART = 1; // object draws some points
const int GOBJ_FILL_ALL  = 2; // object fills in the whole image with opaque colors

const iRect GOBJ_MAX_RANGE(
  iPoint(INT_MIN/2, INT_MIN/2),
  iPoint(INT_MAX/2, INT_MAX/2));

GObj::GObj() {}

iRect
GObj::range(void) const { return GOBJ_MAX_RANGE; }

void
GObj::set_scale(const double k) {}

bool
GObj::get_xloop() const {return false;}

bool
GObj::get_yloop() const {return false;}
