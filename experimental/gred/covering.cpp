#include "covering.h"

Covering::Covering() {
}

Covering Covering::from_rect(Rect<GCoord> rect) {
  GCoord size = GCoord_max - GCoord_min;
  int level = 0;
  while (size >= rect.w && size >= rect.h) {
    size /= 2;
    level++;
  }
  size *= 2;
  level--;
  // now size >= rect, but size/2 < rect.
  SuperCell tlc = SuperCell::from_point(rect.TLC(), level);
  SuperCell brc = SuperCell::from_point(rect.BRC() - Point<GCoord>(1,1), level);
  Point<int> tlc_addr = tlc.to_coord();
  Point<int> brc_addr = brc.to_coord();
  Point<int> cur;
  Covering cov;
  for (cur.x = tlc_addr.x; cur.x <= brc_addr.x; ++cur.x) {
    for (cur.y = tlc_addr.y; cur.y <= brc_addr.y; ++cur.y) {
      cov.cells.push_back(SuperCell::from_coord(cur, level));
    }
  }
  return cov;
}

void Covering::append_cells(std::vector<SuperCell>* vect) {
  for (int i = 0; i < cells.size(); ++i) {
    vect->push_back(cells[i]);
  }
}
