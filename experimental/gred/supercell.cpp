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

Point<int> SuperCell::to_coord() {
  uint64_t srcbit = ROOT;
  uint64_t mask = (id_ - 1) ^ id_; // xxx1000 -> 0001111
  Point<int> p(0,0);
  while (srcbit > mask) {
    p.x <<= 1;
    if (id_ & srcbit) {
      p.x |= 1;
    }
    srcbit >>= 1;
    p.y <<= 1;
    if (id_ & srcbit) {
      p.y |= 1;
    }
    srcbit >>= 1;
  }
  return p;
}

SuperCell SuperCell::from_coord(Point<int> addr, int level) {
  uint64_t srcbit = 1 << (level-1);
  uint64_t dstbit = ROOT;
  uint64_t id = 0;
  while (level > 0) {
    if (addr.x & srcbit) {
      id |= dstbit;
    }
    dstbit >>= 1;
    if (addr.y & srcbit) {
      id |= dstbit;
    }
    dstbit >>= 1;
    srcbit >>= 1;
    level --;
  }
  id |= dstbit;
  return SuperCell(id);
}
