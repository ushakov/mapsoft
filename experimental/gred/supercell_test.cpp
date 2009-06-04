#include <gtest/gtest.h>

#include "supercell.h"

TEST(SuperCell, Parent) {
  EXPECT_EQ(0xe00, SuperCell(0xf80).parent().get_id());
  EXPECT_EQ(0xe00<<1, SuperCell(0xf80<<1).parent().get_id());
  EXPECT_EQ(0x8000000000000000ULL, SuperCell(0x8000000000000000ULL).parent().get_id());
  EXPECT_EQ(0x8000000000000000ULL, SuperCell(0xe000000000000000ULL).parent().get_id());
}

bool InArray(uint64_t id, SuperCell* arr, int size) {
  for (int i = 0; i < size; ++i) {
    if (id == arr[i].get_id()) {
      return true;
    }
  }
  return false;
}

TEST(SuperCell, Subdivide) {
  SuperCell p[4];
  ASSERT_EQ(4, SuperCell(0xf80ULL).subdivide(p));
  EXPECT_TRUE(InArray(0xf20ULL, p, 4));
  EXPECT_TRUE(InArray(0xf60ULL, p, 4));
  EXPECT_TRUE(InArray(0xfa0ULL, p, 4));
  EXPECT_TRUE(InArray(0xfe0ULL, p, 4));
}

TEST(SuperCell, Range) {
  ASSERT_EQ(Rect<GCoord>(0,0,1,1), SuperCell(0x2).range());
  Rect<GCoord> full(GCoord_min, GCoord_min, GCoord_max-GCoord_min, GCoord_max-GCoord_min);
  ASSERT_EQ(full, SuperCell(SuperCell::ROOT).range());
  Rect<GCoord> rect_a, rect_b;
  SuperCell cell(0x2345600);
  rect_a = cell.range();
  rect_b = cell.parent().range();
  EXPECT_TRUE(rect_b.contains(rect_a.TLC()));
  EXPECT_TRUE(rect_b.contains(rect_a.BRC() - Point<GCoord>(1,1)));
}

TEST(SuperCell, FromPoint) {
  Point<GCoord> p(345,345);
  ASSERT_TRUE(SuperCell::from_point(p, 20).range().contains(p));

  p = Point<GCoord>(0, 0);
  ASSERT_TRUE(SuperCell::from_point(p, 10).range().contains(p));

  p = Point<GCoord>(256, 256);
  ASSERT_TRUE(SuperCell::from_point(p, 23).range().contains(p));

  p = Point<GCoord>(GCoord_cnt, GCoord_cnt);
  ASSERT_TRUE(SuperCell::from_point(p, 20).range().contains(p));
  ASSERT_EQ(0xc080000000000000ULL, SuperCell::from_point(p, 4).get_id());

  p = Point<GCoord>(1045, 44556);
  ASSERT_EQ(SuperCell::ROOT, SuperCell::from_point(p, 0).get_id());
}

TEST(SuperCell, LCA) {
  ASSERT_EQ(
	    0x455678000ULL, 
	    SuperCell::LCA(
			   SuperCell(0x45567abc0ULL),
			   SuperCell(0x45567fed0ULL)).get_id());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
