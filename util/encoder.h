//
// Created by xiaofzhao4 on 11/30/2022.
//

#include <string>

#ifndef SMVKV_UTIL_ENCODER_H_
#define SMVKV_UTIL_ENCODER_H_

namespace smvkv {

struct KeyEncoder {
  static std::string Encode(const std::string& key);
  static std::string Decode(const std::string& key);
};

struct SpatialEncoder {
  virtual std::string Encode(const double lat, const double lon) = 0;
  virtual std::string Decode(const std::string& key) = 0;
};

}  // namespace smvkv

#endif  // SMVKV_UTIL_ENCODER_H_
