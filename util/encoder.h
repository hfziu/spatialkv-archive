//
// Created by xiaofzhao4 on 11/30/2022.
//

#include <string>
#include "s2/s2cell_id.h"
#include "s2/s2latlng.h"
#include "spatialkv/dbformat.h"

#ifndef SMVKV_UTIL_ENCODER_H_
#define SMVKV_UTIL_ENCODER_H_

namespace spatialkv {

struct KeyEncoder {
  static std::string Encode(const std::string& key);
  static std::string Decode(const std::string& key);
};

class SpatialEncoder {
 public:
  virtual uint64_t Encode(const Coordinate& coord) const = 0;
};

const SpatialEncoder* S2CellIdEncoder();

}  // namespace spatialkv

#endif  // SMVKV_UTIL_ENCODER_H_
