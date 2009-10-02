#include "supercell.h"

const uint64_t SuperCell::ROOT = (1ULL << 62);

SuperCell SuperCell::from_point(Point<GCoord> p, int level) {
  uint64_t one = ROOT;
  uint64_t id = 0;
  GCoord tlx = GCoord_min;
  GCoord tly = GCoord_min;
  GCoord size = GCoord_max - GCoord_min;
  int cur_level = 0;
  while (cur_level < level) {
    size /= 2;
    if (p.x >= tlx + size) {
      id |= one;
      tlx += size;
    }
    one >>= 1;
    if (p.y >= tly + size) {
      id |= one;
      tly += size;
    }
    one >>= 1;
    cur_level++;
  }
  // Mark the end
  id |= one;
  return SuperCell(id);
}

SuperCell SuperCell::LCA(SuperCell a, SuperCell b) {
  uint64_t mask = ROOT | (ROOT >> 1);
  while ((a.id_ & mask) == (b.id_ & mask)) {
    mask >>= 2;
  }
  // mask is 11000...
  // we need to clean a.id_ at those positions and add 10000...
  mask = (mask - 1) ^ mask;  // 01111...
  uint64_t one = mask + 1;  // 10000...
  mask <<= 1; mask += 1;  // 11111...
  uint64_t new_id = a.id_ & (~mask);
  new_id |= one;
  return SuperCell(new_id);
}
