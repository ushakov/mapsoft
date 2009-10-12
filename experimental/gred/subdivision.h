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
  typedef std::set<SuperCell>::const_iterator const_iterator;

  Subdivision();
  
  // Access to cells of the subdivision.
  const_iterator begin() const { return cells.begin(); }
  const_iterator end() const { return cells.end(); }

  // Supercell coverring the point.
  SuperCell at_point(const Point<GCoord>& p) const;

  // Add supercells covering the input region to the output
  // array. Returns number of elements added.
  int get_covering(const std::vector<SuperCell>& region,
		   std::vector<SuperCell>* output) const;
  
private:
  std::set<SuperCell> cells; // Ordered by id!
};

#endif   // SUBDIVISION_H
