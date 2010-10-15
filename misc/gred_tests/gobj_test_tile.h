#ifndef GOBJ_TEST_TILE_H
#define GOBJ_TEST_TILE_H

#include "gred/iface/gobj.h"

class GObjTestTile: public GObj{
  bool slow;
public:
  GObjTestTile(const bool slow_ = false);
  virtual int draw(iImage &img, const iPoint &origin);
};

#endif
