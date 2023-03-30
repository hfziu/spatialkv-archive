//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "spatialkv/keyformat.h"

#include "util/coding.h"

namespace spatialkv {

std::string SpatialKey::Encode() const {
  std::string result = std::move(std::to_string(type_));
  result.append(
          BigEndianEncoder::EncodeFixed64(spatial_encoder_->Encode(coordinate_)));
  return result;
}

std::string SpatialKey::DebugString() const {
  return "SpatialKey:(" + std::to_string(coordinate_.lat) + "," +
         std::to_string(coordinate_.lng) + ")";
}

}  // namespace spatialkv