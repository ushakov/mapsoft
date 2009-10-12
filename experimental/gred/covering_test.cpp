#include <gtest/gtest.h>

#include "../../core/utils/log.h"
#include "covering.h"

bool Intersects(Rect<GCoord> r1, Rect<GCoord> r2) {
  return !rect_intersect(r1, r2).empty();
}

TEST(Covering, AtMostFourAndCovers) {
  std::vector<GCoord> grid;
  std::vector<SuperCell> arr;
  for(GCoord x = GCoord_min; x < GCoord_max; x += (GCoord_max - GCoord_min) / 7) {
    grid.push_back(x);
  }
  for(int x = 1; x < 6; ++x) {
    for(int y = 1; y < 6; ++y) {
      for (int s = 1; s < 5 && x + s < 7 && y + s < 7; ++s) {
	Rect<GCoord> r(grid[x], grid[y], grid[x+s] - grid[x], grid[y+s] - grid[y]);
	Covering cov = Covering::from_rect(r);
	arr.clear();
	cov.append_cells(&arr);
	EXPECT_LE(arr.size(), 4);
	int level = arr[0].level();
	for (int i = 0; i < arr.size(); ++i) {
	  EXPECT_EQ(level, arr[i].level());
	  EXPECT_PRED2(Intersects, arr[i].range(), r);
	}
      }
    }
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
