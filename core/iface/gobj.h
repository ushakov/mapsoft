#ifndef GOBJ_IFACE_H
#define GOBJ_IFACE_H

#include "lib2d/rect.h"
#include "lib2d/image.h"

class GObj{
  public:
  virtual iImage draw(const iRect &range) = 0;
  virtual iRect range(void) = 0;
};

#endif
