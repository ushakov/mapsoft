#ifndef GPLANE_H
#define GPLANE_H

#include <limits.h>

#include "../../core/lib2d/rect.h"
#include "../../core/lib2d/image.h"

const extern int GCoordMin;
const extern int GCoordMax;

class GPlane{
  public:
  virtual iImage draw(const iRect &range) = 0;
};



// here are some simple planes for test purposes

class GPlaneTestTile: public GPlane{
  iImage draw(const iRect &range);
};

class GPlaneTestTileSlow: public GPlane{
  iImage draw(const iRect &range);
};

class GPlaneSolidFill: public GPlane{
  int color;
  public:
  GPlaneSolidFill(int c=0xFF000000): color(c) {}
  iImage draw(const iRect &range);
};

class GPlaneTestGrid: public GPlane{
  iImage draw(const iRect &range);
};

class GPlaneTestGridSlow: public GPlane{
  iImage draw(const iRect &range);
};

#endif
