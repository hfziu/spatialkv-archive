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

class IntegerEncoder {
 public:
  [[nodiscard]] static std::string EncodeFixed64(uint64_t value);
  [[nodiscard]] static uint64_t DecodeFixed64(std::string_view value);

 private:
  static std::string valid_chars_;
};

std::string IntegerEncoder::valid_chars_ =
      "0123456789<>"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

// Encode a 64-bit integer into a fixed-length string.
std::string IntegerEncoder::EncodeFixed64(uint64_t value){
  auto buf = std::make_unique<char[]>(11);
  for (int i = 10; i >= 0; --i) {
    buf[i] = valid_chars_[value % valid_chars_.size()];
    value /= valid_chars_.size();
  }
  return {buf.get()};
}

uint64_t IntegerEncoder::DecodeFixed64(std::string_view value){
  uint64_t result = 0;
  for (auto c : value) {
    result *= valid_chars_.size();
    result += valid_chars_.find(c);
  }
  return result;
}

}  // namespace

}  // namespace spatialkv

#endif  // SPATIALKV_UTIL_CODING_H_
