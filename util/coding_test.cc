//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "gtest/gtest.h"
#include "leveldb/comparator.h"
#include "leveldb/slice.h"
#include "util/coding.h"

namespace spatialkv {

auto comparator = leveldb::BytewiseComparator();

TEST(Coding, BigEndianEncoder) {
  auto& Encode = BigEndianEncoder::EncodeFixed64;
  auto& Decode = BigEndianEncoder::DecodeFixed64;
  for (int i = 0; i < 100; i++) {
    auto encoded = Encode(i);
    auto decoded = Decode(encoded);
    ASSERT_EQ(i, decoded);
  }
  for (int i = 0; i < 100; i++) {
    auto encoded = Encode(0xffffffffffffffff - i);
    auto decoded = Decode(encoded);
    ASSERT_EQ(0xffffffffffffffff - i, decoded);
  }
  // Verify that the encoded string preserves the ordering of the input.
  // We use LevelDB's default BytewiseComparator.
  for (int i = 0; i < 0xffff; i++) {
    auto encoded1 = Encode(i);
    auto encoded2 = Encode(i + 1);
    ASSERT_TRUE(0 > comparator->Compare(encoded1, encoded2));
    auto encoded3 = Encode(0xffffffffffffffff - i);
    auto encoded4 = Encode(0xffffffffffffffff - i - 1);
    ASSERT_TRUE(0 < comparator->Compare(encoded3, encoded4));
  }
}

}  // namespace spatialkv