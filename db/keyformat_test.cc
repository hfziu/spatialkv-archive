//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "spatialkv/keyformat.h"

#include "gtest/gtest.h"
#include "util/coding.h"

namespace spatialkv {

auto* s2encoder = S2CellIdEncoder();

TEST(KeyFormat, ExampleLocation0) {
  for (int lat = -90; lat <= 90; lat++) {
    for (int lng = -180; lng <= 180; lng++) {
      auto key =
          SpatialKey({static_cast<Lat>(lat), static_cast<Lng>(lng)}, s2encoder);
      ASSERT_EQ(key.Encode().substr(1),  // skip key type
                BigEndianEncoder::EncodeFixed64(s2encoder->Encode(
                    {static_cast<Lat>(lat), static_cast<Lng>(lng)})));
    }
  }
}

}  // namespace spatialkv
