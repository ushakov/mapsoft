#ifndef GOBJ_TEST_TILE_H
#define GOBJ_TEST_TILE_H

#include "iface/gobj.h"

class GObjTestTile: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

class GObjTestTileSlow: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

#endif
