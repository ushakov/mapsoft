#ifndef SUPERCELL_H
#define SUPERCELL_H

#include "../../core/lib2d/rect.h"
#include "gcoord.h"

/* id encoding: (uint64_t) meaningful part is from the highest bit to
 * the last 1: xxxxxxxxxx1000000...
 *
 * This leaves us with 63 bits of which we use only 62 (2*31).  At
 * highest level (0) the whole plane is one cell. It has empty address
 * (id 100...00).
 * 
 * Each of the cells of level N is a subdivision of some cell of level
 * N-1.  If the cell's address is X then the four its children have
 * addresses X00 through X11 in this order:
 *
 * 00 10
 * 01 11
 */  
#include <boost/operators.hpp>

class SuperCell : 
  public boost::less_than_comparable<SuperCell>,
  public boost::equality_comparable<SuperCell>
{
 private:
  uint64_t id;
 public:
  static const uint64_t ROOT;

  SuperCell() {
    id = ROOT;
  }

  explicit SuperCell(const uint64_t& id) {
    this->id = id;
  }

  SuperCell parent() {
    if (id == ROOT) return SuperCell(ROOT);
    uint64_t mask = (id - 1) ^ id;  // xxx1000 -> 0001111
    mask = (mask << 2) + 3;  // all tail and two lower bits of address masked
    uint64_t new_id = id & (~mask);  // parent address, not encoded
    new_id |= (mask >> 1) + 1;
    return SuperCell(new_id);
  }

  // fills in arr, returns number of children written (which is 4 if this cell
  // is at non-last level, and 0 if it's on the last).
  int subdivide(SuperCell* arr) {
    if (id & 0x3 != 0) {
      // already at last level -- nothing can be done
      return 0;
    }
    uint64_t mask = (id - 1) ^ id;  // xxx1000 -> 0001111
    uint64_t addr = id & (~mask);  // clean address
    mask >>= 2;  // mask for children
    uint64_t one = mask + 1;  // a 1 at lower position of the new address
    addr |= (one >> 1);  // address template (aka child 00)
    arr[0] = SuperCell(addr);
    for (int i = 1; i < 4; ++i) {
      addr += one;
      arr[i] = SuperCell(addr);
    }
    return 4;
  }

  int level() {
    uint64_t mask = (id - 1) ^ id;  // xxx1000 -> 0001111
    int antilevel = 0;
    while (mask != 0) {
      antilevel++;
      mask >>= 2;
    }
    return 32 - antilevel;
  }

  uint64_t get_id() {
    return id;
  }

  Rect<GCoord> range() {
    GCoord tlx = GCoord_min;
    GCoord tly = GCoord_min;
    GCoord size = GCoord_max - GCoord_min;
    uint64_t mask = (id - 1) ^ id;
    uint64_t curbit = ROOT;
    while (curbit > mask) {
      size /= 2;
      if (id & curbit) {
	tlx += size;
      }
      curbit >>= 1;
      if (id & curbit) {
	tly += size;
      }
      curbit >>= 1;
    }
    return Rect<GCoord>(tlx, tly, size, size);
  }

  static SuperCell from_point(Point<GCoord> p, int level);
  static SuperCell LCA(SuperCell a, SuperCell b);

  bool operator== (SuperCell b) {
    return id == b.id;
  }

  bool operator< (const SuperCell& b) {
    return id < b.id;
  }
};

#endif  // SUPERCELL_H
