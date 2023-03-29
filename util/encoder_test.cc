//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "gtest/gtest.h"

#include "encoder.h"

namespace spatialkv {

TEST(Encoder, ExampleLocation0) {
  auto encoder = std::make_unique<S2CellIdEncoder>();
  std::cout << encoder->Encode({0, 1}) << std::endl;
}

}  // namespace spatialkv

