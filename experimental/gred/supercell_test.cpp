#include <sstream>
#include <gtest/gtest.h>

#include "supercell.h"

TEST(SuperCell, Parent) {
  EXPECT_EQ(0xe00, SuperCell(0xf80).parent().id());
  EXPECT_EQ(0xe00<<1, SuperCell(0xf80<<1).parent().id());
  EXPECT_EQ(0x8000000000000000ULL, SuperCell(0x8000000000000000ULL).parent().id());
  EXPECT_EQ(0x8000000000000000ULL, SuperCell(0xe000000000000000ULL).parent().id());
}

bool InArray(uint64_t id, SuperCell* arr, int size) {
  for (int i = 0; i < size; ++i) {
    if (id == arr[i].id()) {
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
  ASSERT_EQ(Rect<GCoord>(GCoord_min,GCoord_min,1,1), SuperCell(0x4).range());
  Rect<GCoord> full(GCoord_min, GCoord_min, GCoord_max-GCoord_min, GCoord_max-GCoord_min);
  ASSERT_EQ(full, SuperCell(SuperCell::ROOT).range());
  Rect<GCoord> rect_a, rect_b;
  SuperCell cell(0x2345300);
  rect_a = cell.range();
  rect_b = cell.parent().range();
  EXPECT_TRUE(point_in_rect(rect_a.TLC(), rect_b));
  EXPECT_TRUE(point_in_rect(rect_a.BRC() - Point<GCoord>(1,1), rect_b));
}

TEST(SuperCell, FromPoint) {
  Point<GCoord> p(345,345);
  SuperCell cell;

  cell = SuperCell::from_point(p, 20);
  EXPECT_TRUE(point_in_rect(p, cell.range()));
  EXPECT_EQ(20, cell.level());
 
  p = Point<GCoord>(0, 0);
  EXPECT_TRUE(point_in_rect(p, SuperCell::from_point(p, 10).range()));

  p = Point<GCoord>(256, 256);
  EXPECT_TRUE(point_in_rect(p, SuperCell::from_point(p, 23).range()));

  p = Point<GCoord>(GCoord_cnt, GCoord_cnt);
  EXPECT_TRUE(point_in_rect(p, SuperCell::from_point(p, 20).range()));
  EXPECT_EQ(0x6040000000000000ULL, SuperCell::from_point(p, 4).id());

  p = Point<GCoord>(1045, 44556);
  EXPECT_EQ(SuperCell::ROOT, SuperCell::from_point(p, 0).id());
  EXPECT_EQ(0, SuperCell(SuperCell::ROOT).level());

  EXPECT_EQ(SuperCell::last_level, SuperCell(0x1).level());
}

TEST(SuperCell, LCA) {
  EXPECT_EQ(
	    0x455674000ULL, 
	    SuperCell::LCA(
			   SuperCell(0x455675b60ULL),
			   SuperCell(0x455677e60ULL)).id());
}

std::string hex_repr(uint64_t t) {
  std::ostringstream s;
  s << std::hex << t;
  return s.str();
}

TEST(SuperCell, FromCoord) {
  Point<int> p(1, 1);
  SuperCell cell = SuperCell::from_coord(p, 1);
  EXPECT_EQ("7000000000000000", hex_repr(cell.id()));
  p = Point<int>(2, 3);
  cell = SuperCell::from_coord(p, 2);
  EXPECT_EQ("6c00000000000000", hex_repr(cell.id()));
}

TEST(SuperCell, ToCoord) {
  SuperCell cell(0x7000000000000000ULL);
  EXPECT_EQ(Point<int>(1,1), cell.to_coord());
  cell = SuperCell(0x6c00000000000000ULL);
  EXPECT_EQ(Point<int>(2,3), cell.to_coord());
}

TEST(SuperCell, ToFromCoord) {
  for (uint64_t i = 3453566ULL; i < 34535660000ULL; i += 3453566000ULL) {
    SuperCell cell(i);
    Point<int> p = cell.to_coord();
    SuperCell parent = cell.parent();
    EXPECT_EQ(p.x / 2, parent.to_coord().x);
    EXPECT_EQ(p.y / 2, parent.to_coord().y);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
