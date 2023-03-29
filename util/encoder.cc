//
// Created by xiaofzhao4 on 11/30/2022.
//

#include "encoder.h"

namespace spatialkv {

uint64_t S2CellIdEncoder::Encode(const Coordinate& coord) {
  // Given a latitude-longitude coordinate, return the corresponding S2CellId.
  auto cell_id = S2CellId(S2LatLng::FromDegrees(coord.lat, coord.lng));
  return cell_id.id();
}

} // namespace spatialkv