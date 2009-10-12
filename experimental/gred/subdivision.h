#ifndef SUBDIVISION_H
#define SUBDIVISION_H

// Abstracts a subdivision of GPlane into supercells of different
// levels. Each point of the plane belongs to exactly one supercell of
// the subdivision. Given a point, we can find the nearest enclosing
// cell in the subdivision. Given a region (as a vector of
// supercells), we can find a vector of supercells in the division
// that fully cover that region.
class Subdivision {
public:
  typedef std::set<SuperCell>::const_iterator iterator;

  Subdivision();
  
  // Access to cells of the subdivision.
  iterator begin() const { return cells.begin(); }
  iterator end() const { return cells.end(); }

  // Supercell covering the point.
  SuperCell at_point(const Point<GCoord>& p) const;

  // Add supercells covering the input region to the output
  // array. Returns number of elements added.
  int get_covering(const std::vector<SuperCell>& region,
		   std::vector<SuperCell>* output) const;

  // Replace currect cell with its 4-cell subdivision. Return iterator
  // pointing at the first of inserted cells.
  iterator subdivide(iterator iter);
  
  // Replace a subdivided cell with a whole cell (inverse of
  // subdivide()). Requires: iter points to the last of the four
  // cells. Returns: iterator pointng at the whole cell.
  iterator undivide(iterator iter);
  
private:
  std::set<SuperCell> cells; // Ordered by id!

  iterator get_nearest_cell_up(SuperCell cell) const;
};

#endif   // SUBDIVISION_H
