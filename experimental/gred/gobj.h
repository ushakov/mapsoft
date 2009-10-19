#ifndef GOBJ_H
#define GOBJ_H

#include <limits.h>

#include "../../core/lib2d/rect.h"
#include "../../core/lib2d/image.h"

const extern int GCoordMin;
const extern int GCoordMax;

class GObj{
  public:
  virtual iImage draw(const iRect &range) = 0;
  virtual iRect range(void);
};



// here are some simple objects for test purposes

class GObjTestTile: public GObj{
  iImage draw(const iRect &range);
};

class GObjTestTileSlow: public GObj{
  iImage draw(const iRect &range);
};

class GObjSolidFill: public GObj{
  int color;
  public:
  GObjSolidFill(int c=0xFF000000): color(c) {}
  iImage draw(const iRect &range);
};

class GObjTestGrid: public GObj{
  iImage draw(const iRect &range);
};

class GObjTestGridSlow: public GObj{
  iImage draw(const iRect &range);
};

#endif
