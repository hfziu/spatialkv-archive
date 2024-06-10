//
// Created by Xiaofei ZHAO on 4/6/2021.
//

#include "curve.h"

#include "gtest/gtest.h"

#include <string>

namespace spatial {

char* EncodeBitNumber(uint64_t num) {
  char* buffer = new char[8];
  leveldb::EncodeFixed64(buffer, num);
  return buffer;
}

using SpatialTable = std::map<char*, int, spatial::SpatialBitComparator>;

// TODO: enable this test
TEST(SpaceCurveTet, NextIdx) {
  auto p1 = NextIdx(EncodeBitNumber(0x0281234567890abc));
  auto p2 = EncodeBitNumber(0x02c1234567890abc);
  ASSERT_EQ(*p1, *p2);
}

TEST(SpaceCurveTest, LeastSignificantSetBit) {
  // SpatialBitComparator cmp;
  ASSERT_EQ(61, SpatialBitComparator::LSB64(4));
  ASSERT_EQ(0, SpatialBitComparator::LSB64(1ull << 63));
  for (int i = 0; i < 64; i++) {
    ASSERT_EQ(i, SpatialBitComparator::LSB64(1ull << (63 - i)));
  }
//  ASSERT_EQ(14, SpatialBitComparator::LSB64(0x0003000000000000));
  ASSERT_EQ(1ull << 48 | 1ull << 49, 0x0003000000000000);
  ASSERT_EQ(14, SpatialBitComparator::LSB64(1ull << 49 | 1ull << 48));
}

TEST(SpaceCurveTest, SpatialBitComparator) {
  SpatialBitComparator cmp;
  // Case 0: a and b are identical
  // length = 8 bits, a = b = 0x08aaull << 60
  char* a = EncodeBitNumber(0x08aaull << 60);
  char* b = EncodeBitNumber(0x08aaull << 60);
  ASSERT_FALSE(cmp(a, b));
  ASSERT_FALSE(cmp(b, a));

  // Case 1: same length = 15 bits, c = 0x0fabcd1234000000, d = 0x0fabce1234000000
  char* c = EncodeBitNumber(0x0fabcd1234000000ull);
  char* d = EncodeBitNumber(0x0fabce1234000000ull);
  ASSERT_TRUE(cmp(c, d));
  ASSERT_FALSE(cmp(d,c));

  // Case 2: the first different bit is after min_length
  // e = 0x18abcdef12340000, f = 0x1fabcdef22340000
  char* e = EncodeBitNumber(0x18abcdef12340000ull);
  char* f = EncodeBitNumber(0x1fabcdef22340000ull);
  ASSERT_TRUE(cmp(e, f));
  ASSERT_FALSE(cmp(f,e));
}

TEST(SpaceCurveTest, SpatialTable) {
  SpatialTable table;
  char* a = EncodeBitNumber(0x0281234567890abc);
  char* b = EncodeBitNumber(0x02c1234567890abc);
  char* c = EncodeBitNumber(0x38d1234567890abc);
  table.emplace(a, 1);
  table.emplace(b, 2);
  table.emplace(c, 3);
  SpatialTable::const_iterator start = table.lower_bound(a);
  SpatialTable::const_iterator end = table.upper_bound(a);
  ASSERT_EQ(*a, *(start->first));
  ASSERT_EQ(1, start->second);
  ASSERT_EQ(*b, *(end->first));
  ASSERT_EQ(2, end->second);
}

TEST(SpaceCurveTest, HilbertEncode) {
  // TODO
  Hilbert hilbert(4);
  Linear t_int;
  Coordinate coord(5.0, 100.0);
  ASSERT_TRUE(hilbert.MapInverse(8, 12, &t_int));
  ASSERT_EQ(t_int, 186);
  ASSERT_TRUE(hilbert.MapInverse(coord, &t_int));
  ASSERT_EQ(t_int, 186);
  ASSERT_TRUE(hilbert.MapInverse(8, 14, &t_int));
  ASSERT_EQ(t_int, 188);
  ASSERT_TRUE(hilbert.MapInverse(4, 9, &t_int));
  ASSERT_EQ(t_int, 219);
  ASSERT_TRUE(hilbert.MapInverse(4, 14, &t_int));
  ASSERT_EQ(t_int, 198);

  std::string t_str;
  ASSERT_TRUE(hilbert.MapInverse(8, 12, &t_str));
  ASSERT_EQ(t_str, "10111010");
}

TEST(SpaceCurveTest, HilbertDecode) {
  // TODO
  Hilbert hilbert(4);
  Linear y, x;
  ASSERT_TRUE(hilbert.Map(187, &x, &y));
  ASSERT_EQ(y, 13);
  ASSERT_EQ(x, 8);
}

TEST(SpaceCurveTest, GeohashEncode) {
  Geohash geohash(15);
  Linear t_int;
  Coordinate coord(22.3338741, 114.184873);
  ASSERT_TRUE(geohash.MapInverse(coord, &t_int));
  ASSERT_EQ(t_int, 953538179);

  std::string t_str;
  ASSERT_TRUE(geohash.MapInverse(coord, &t_str));
  ASSERT_EQ(t_str, "111000110101011101011010000011");
}

TEST(SpaceCurveTest, GeohashDecode) {
  Geohash geohash(15);
  Linear t = 953538179;
  Coordinate coord(0, 0);
  ASSERT_TRUE(geohash.Map(t, &coord));
  // TODO(xf): check longitude and latitude
  // Temp: result: coord.x = 22.3324585, coord.y = 114.186401
}

}  // namespace spatial

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}