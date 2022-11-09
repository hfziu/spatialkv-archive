//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "gtest/gtest.h"

#include "encoder.h"

namespace spatialkv {

auto s2encoder = S2CellIdEncoder();

TEST(Encoder, ExampleLocation0) {

  std::cout << s2encoder->Encode({0, 1}) << std::endl;
}

}  // namespace spatialkv

