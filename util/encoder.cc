//
// Created by xiaofzhao4 on 11/30/2022.
//

#include "util/encoder.h"
#include "util/no_destructor.h"

namespace spatialkv {

namespace {

class S2CellIdEncoderImpl : public SpatialEncoder {
 public:
  [[nodiscard]] uint64_t Encode(const Coordinate& coord) const override;
};

uint64_t S2CellIdEncoderImpl::Encode(const Coordinate& coord) const {
  // Given a latitude-longitude coordinate, return the corresponding S2CellId.
  auto cell_id = S2CellId(S2LatLng::FromDegrees(coord.lat, coord.lng));
  return cell_id.id();
}

}  // namespace

const SpatialEncoder* S2CellIdEncoder() {
  static NoDestructor<S2CellIdEncoderImpl> encoder;
  return encoder.get();
}

} // namespace spatialkv