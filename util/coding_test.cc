//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "gtest/gtest.h"
#include "leveldb/comparator.h"
#include "leveldb/slice.h"
#include "util/coding.h"

namespace spatialkv {

auto comparator = leveldb::BytewiseComparator();

TEST(Coding, IntegerEncoder) {
  auto& Encode = IntegerEncoder::EncodeFixed64;
  auto& Decode = IntegerEncoder::DecodeFixed64;
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
  for (int i = 0; i < 1000; i++) {
    auto encoded1 = Encode(i);
    auto encoded2 = Encode(i + 1);
    ASSERT_TRUE(0 > comparator->Compare(encoded1, encoded2));
  }
}

}  // namespace spatialkv