//
// Created by xiaofzhao4 on 12/2/2022.
//

#include "gtest/gtest.h"
#include "s2/s2cell_id.h"
#include "s2/s2latlng.h"
#include "s2/s2point.h"
#include "util/s2_util.h"

TEST(S2Point, Empty) { S2Point point; }

TEST(S2Point, PrintCellId) {
  S2LatLng latlng = S2LatLng::FromDegrees(10, -160);
  S2Point point = latlng.ToPoint();
  // get the point's cell id
  auto cell_id = S2CellId(point);
  // print the cell id's binary representation
  std::cout << spatialkv::UInt64ToDebugString(cell_id.id()) << std::endl;
  // print the level-2 prefix of the cell id
  std::cout << spatialkv::UInt64ToDebugString(
                   spatialkv::CellIdPrefix(cell_id.id(), 2))
            << std::endl;
}