#ifndef POLY_TESTER_H
#define POLY_TESTER_H

#include <vector>
#include "2d/rect.h"
#include "2d/line.h"

/// fast integer polygon tester
struct poly_tester{
private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  iLine border;
public:
  poly_tester(const iLine & brd);

  /// Is point inside polygon?
  bool test(const iPoint & p) const;

  /// Does polygon and rect have non-null intersection?
  /// Inacurate! Use rect_crop instead.
  bool test(const iRect & r) const;

  /// Distance to the nearest border on the right.
  /// Positive for inner points, negative for outer ones.
  /// Not used?
  int nearest_border (const iPoint & p) const;
};

#endif
