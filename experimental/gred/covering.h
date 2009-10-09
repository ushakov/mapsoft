#ifndef COVERING_H
#define COVERING_H

#include <vector>
#include "supercell.h"
#include "../../core/lib2d/rect.h"

// Covering is a disjoint union of supercells.  Covering::FromRect
// creates a covering that is:
//   - at most twice as large than input rect by any coordinate, and
//   - contains at most 4 cells.
class Covering {
 public:
  Covering();

  void append_cells(std::vector<SuperCell>* vect);
  static Covering from_rect(Rect<GCoord> rect);
 private:
  std::vector<SuperCell> cells;
};

#endif  // COVERING_H
