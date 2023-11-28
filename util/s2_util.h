//
// Created by Xiaofei ZHAO on 28/11/2022.
//

#ifndef SPATIALKV_UTIL_S2_UTIL_H_
#define SPATIALKV_UTIL_S2_UTIL_H_

#include "util/encoder.h"

namespace spatialkv {

// Convert a uint64_t to its binary representation.
std::string UInt64ToDebugString(uint64_t value) {
  std::string result;
  result.resize(64);
  for (int i = 63; i >= 0; --i) {
    result[i] = (value & 1) ? '1' : '0';
    value >>= 1;
  }
  return result;
}

uint64_t CellIdPrefix(uint64_t id, int level) {
  // the first 3 + 2 * level bits of id are left unchanged;
  // the (4 + 2 * level)th bit is set to 1;
  // the remaining bits are set to 0.
  uint64_t mask = 0xffffffffffffffff;
  mask <<= (61 - 2 * level);
  id &= mask;
  id |= (1ull << (60 - 2 * level));
  return id;
}

}  // namespace spatialkv

#endif  // SPATIALKV_UTIL_S2_UTIL_H_
