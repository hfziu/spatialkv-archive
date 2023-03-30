//
// Created by xiaofzhao4 on 3/29/2023.
//

#ifndef SPATIALKV_UTIL_CODING_H_
#define SPATIALKV_UTIL_CODING_H_

#include <memory>
#include <string>
#include <string_view>

#include "util/no_destructor.h"

namespace spatialkv {

namespace {

class BigEndianEncoder {
 public:
  [[nodiscard]] static std::string EncodeFixed64(uint64_t value);

  [[nodiscard]] static uint64_t DecodeFixed64(std::string_view big_endian);

  static void PutFixed64(std::string *dst, uint64_t value);
};

// Encode a 64-bit integer into a fixed-length string.
std::string BigEndianEncoder::EncodeFixed64(uint64_t value) {
  std::array<unsigned char, 8> big_endian{};
  for (int i = 7; i >= 0; --i) {
    big_endian[i] = static_cast<char>((value)&0xff);
    value >>= 8;
  }
  return {big_endian.begin(), big_endian.end()};
}

uint64_t BigEndianEncoder::DecodeFixed64(std::string_view big_endian) {
  uint64_t value = 0;
  for (auto c : big_endian) {
    value <<= 8;
    value += static_cast<uint8_t>(c);
  }
  return value;
}

void BigEndianEncoder::PutFixed64(std::string *dst, uint64_t value) {
  dst->append(EncodeFixed64(value));
}

}  // namespace

}  // namespace spatialkv

#endif  // SPATIALKV_UTIL_CODING_H_
