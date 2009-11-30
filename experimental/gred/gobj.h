#ifndef GOBJ_H
#define GOBJ_H

#include <limits.h>

#include "iface/gobj.h"

#include "../../core/lib2d/rect.h"
#include "../../core/lib2d/image.h"

//const extern int GCoordMin;
//const extern int GCoordMax;

// some simple objects for test purposes

class GObjTestTile: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

class GObjTestTileSlow: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

class GObjSolidFill: public GObj{
  int color;
  public:
  GObjSolidFill(int c=0xFF000000): color(c) {}
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

class GObjTestGrid: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

class GObjTestGridSlow: public GObj{
  virtual iImage draw(const iRect &range);
  virtual iRect range(void);
};

#endif
