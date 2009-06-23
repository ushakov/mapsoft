#ifndef GPLANE_H
#define GPLANE_H

#include <limits.h>

#include "../../core/lib2d/rect.h"
#include "../../core/lib2d/image.h"

const extern int GCoordMin;
const extern int GCoordMax;

class GPlane{
  public:
  virtual Image<int> draw(const Rect<int> &range) const = 0;
};



// here are some simple planes for test purposes

class GPlaneTestTile: public GPlane{
  Image<int> draw(const Rect<int> &range) const;
};

class GPlaneTestTileSlow: public GPlane{
  Image<int> draw(const Rect<int> &range) const;
};

class GPlaneSolidFill: public GPlane{
  int color;
  public:
  GPlaneSolidFill(int c=0xFF000000): color(c) {}
  Image<int> draw(const Rect<int> &range) const;
};

class GPlaneTestGrid: public GPlane{
  Image<int> draw(const Rect<int> &range) const;
};

class GPlaneTestGridSlow: public GPlane{
  Image<int> draw(const Rect<int> &range) const;
};

#endif
