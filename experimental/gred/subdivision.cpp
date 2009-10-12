#include "subdivision.h"

Subdivision::Subdivision() {
}
  
// Supercell covering the point.
SuperCell Subdivision::at_point(const Point<GCoord>& p) const {
  SuperCell point = SuperCell::from_point(p, SuperCell::last_level);
  iterator it = cells.lower_bound(point);
  if (*it == point) {
    return *it;
  }
  // Otherwise, lower_bound returned the first element in cells that
  // it larger than point. We need, however, the last one that it
  // smaller than point.
  it--;
  return *it;
}

// Add supercells covering the input region to the output
// array. Returns number of elements added.
int get_covering(const std::vector<SuperCell>& region,
		 std::vector<SuperCell>* output) const {
  
}

  // Replace currect cell with its 4-cell subdivision. Return iterator
  // pointing at the first of inserted cells.
  iterator subdivide(iterator iter);
  
  // Replace a subdivided cell with a whole cell (inverse of
  // subdivide()). Requires: iter points to the last of the four
  // cells. Returns: iterator pointng at the whole cell.
  iterator undivide(iterator iter);

SuperCell::iterator SuperCell::get_nearest_cell_up (SuperCell cell) const {
  
  iterator it = cells.lower_bound(cell);
 
}
