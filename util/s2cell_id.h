//
// Created by xiaofzhao4 on 12/15/2022.
//

#ifndef SMVKV_UTIL_S2CELL_ID_H_
#define SMVKV_UTIL_S2CELL_ID_H_

#include "encoder.h"

namespace smvkv {

class S2CellIdEncoder : public SpatialEncoder {
 public:
  std::string Encode(const std::string& key) override;
  std::string Decode(const std::string& key) override;
};

} // namespace smvkv

#endif  // SMVKV_UTIL_S2CELL_ID_H_
