#ifndef SUPERCELL_H
#define SUPERCELL_H

#include <inttypes.h>
#include "../../core/lib2d/rect.h"
#include "gcoord.h"

/* id encoding: (uint64_t), meaningful part is from the second-highest
 * bit (31st) to the last 1: 0xxxxxxxxxx1000000... Unencoded ids are
 * called addresses, encoded ids are called ids.
 *
 * This leaves us with 62 bits (2**31).  At highest level (0) the
 * whole plane is one cell. It has empty address (id 0100...00). This
 * also make valid cell ids a contiguous region in uint64_t space.
 * 
 * Each of the cells of level N is a subdivision of some cell of level
 * N-1.  If the cell's address is X then the four its children have
 * addresses X00 through X11 in this order:
 *
 * 00 10
 * 01 11
 *
 * This way, the cell X's id in right in the middle of the range of
 * all ids that are descendants of cell X.
 */  
#include <boost/operators.hpp>

class SuperCell : 
  public boost::less_than_comparable<SuperCell>,
  public boost::equality_comparable<SuperCell>
{
 private:
  uint64_t id_;
 public:
  static const uint64_t ROOT;
  static const int last_level;

  SuperCell() {
    id_ = ROOT;
  }

  explicit SuperCell(const uint64_t& id) {
    this->id_ = id;
  }

  SuperCell parent() {
    if (id_ == ROOT) return SuperCell(ROOT);
    uint64_t mask = (id_ - 1) ^ id_;  // xxx1000 -> 0001111
    mask = (mask << 2) + 3;  // all tail and two lower bits of id masked
    uint64_t new_id = id_ & (~mask);  // parent address, not encoded
    new_id |= (mask >> 1) + 1;
    return SuperCell(new_id);
  }

  // fills in arr, returns number of children written (which is 4 if this cell
  // is at non-last level, and 0 if it's on the last).
  int subdivide(SuperCell* arr) {
    if (id_ & 0x1 != 0) {
      // already at last level -- nothing can be done
      return 0;
    }
    uint64_t mask = (id_ - 1) ^ id_;  // xxx1000 -> 0001111
    uint64_t addr = id_ & (~mask);  // clean address
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
    uint64_t mask = (id_ - 1) ^ id_;  // xxx1000 -> 0001111
    int antilevel = 0;
    while (mask != 0) {
      antilevel++;
      mask >>= 2;
    }
    return last_level - antilevel + 1;
  }

  uint64_t id() {
    return id_;
  }

  Rect<GCoord> range() {
    GCoord tlx = GCoord_min;
    GCoord tly = GCoord_min;
    GCoord size = GCoord_max - GCoord_min;
    uint64_t mask = (id_ - 1) ^ id_;
    uint64_t curbit = ROOT;
    while (curbit > mask) {
      size /= 2;
      if (id_ & curbit) {
	tlx += size;
      }
      curbit >>= 1;
      if (id_ & curbit) {
	tly += size;
      }
      curbit >>= 1;
    }
    return Rect<GCoord>(tlx, tly, size, size);
  }

  void cell_range(SuperCell* low, SuperCell* high) {
    uint64_t mask = (id_ - 1) ^ id_;   // xxx1000... -> 0001111
    low->id_ = id_ & (~mask) + 1;      // xxx000...1
    high->id_ = id_ | mask + 1;        // xxx111...1 + 1, first not in range     
  }

  // Make x,y coordinates of the cell in the grid for current level,
  // and the cell from such coordinates.
  Point<int> to_coord();
  static SuperCell from_coord(Point<int> addr, int level);

  static SuperCell from_point(Point<GCoord> p, int level);
  static SuperCell LCA(SuperCell a, SuperCell b);

  bool operator== (SuperCell b) {
    return id_ == b.id_;
  }

  bool operator< (const SuperCell& b) {
    return id_ < b.id_;
  }
};

#endif  // SUPERCELL_H
