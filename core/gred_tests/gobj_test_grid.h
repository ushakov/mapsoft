#ifndef GOBJ_TEST_GRID_H
#define GOBJ_TEST_GRID_H

#include "iface/gobj.h"

// Grid for test purposes
class GObjTestGrid: public GObj{
  int delay;
public:
  GObjTestGrid(const int delay = 0);
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};


#endif
