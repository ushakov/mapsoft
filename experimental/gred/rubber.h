#ifndef RUBBER_H
#define RUBBER_H

#include "gplane.h"


const int PREF_XMOUSE=1;
const int PREF_YMOUSE=2;

class RubberSegment{
public:
    int           r1, r2;
    Point<GCoord> p1, p2;

    RubberSegment(
      const Point<GCoord> & p1_, const int r1_,
      const Point<GCoord> & p2_, const int r2_): p1(p1_), r1(r1_), p2(p2_), r2(r2_){ }

    Point<GCoord> get1(Point<GCoord> pointer, Point<GCoord> offset) const {
      return Point<GCoord> ((r1 & PREF_XMOUSE)? (p1.x+pointer.x) : (p1.x-offset.x),
                            (r1 & PREF_YMOUSE)? (p1.y+pointer.y) : (p1.y-offset.y));
    }
    Point<GCoord> get2(Point<GCoord> pointer, Point<GCoord> offset) const {
      return Point<GCoord> ((r2 & PREF_XMOUSE)? (p2.x+pointer.x) : (p2.x-offset.x),
                            (r2 & PREF_YMOUSE)? (p2.y+pointer.y) : (p2.y-offset.y));
    }
};



#endif
