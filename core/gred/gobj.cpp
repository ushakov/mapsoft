#include "gobj.h"
#include <limits.h>

// return codes for draw function
const int GOBJ_FILL_NONE = 0; // object draws nothing
const int GOBJ_FILL_PART = 1; // object draws some points
const int GOBJ_FILL_ALL  = 2; // object fills in the whole image with opaque colors

const iRect GOBJ_MAX_RANGE(
  iPoint(INT_MIN/2, INT_MIN/2),
  iPoint(INT_MAX/2, INT_MAX/2));

const int GObj::TILE_SIZE;
