#ifndef GOBJ_TEST_GRID_H
#define GOBJ_TEST_GRID_H

#include "gred/iface/gobj.h"

// Grid for test purposes
class GObjTestGrid: public GObj{
  int delay;
public:
  GObjTestGrid(const int delay = 0);
  virtual int draw(iImage &img, const iPoint &origin);
};


#endif
