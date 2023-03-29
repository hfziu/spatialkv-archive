//
// Created by xiaofzhao4 on 12/15/2022.
//

#ifndef SMVKV_DB_FORMAT_H_
#define SMVKV_DB_FORMAT_H_

#include <string>

namespace spatialkv {

using StreamID = uint32_t;
using SequenceNumber = uint64_t;
using ValidTime = uint64_t;
using Lat = double;
using Lng = double;

struct Coordinate {
  Lat lat;
  Lng lng;
};

}  // namespace spatialkv

#endif  // SMVKV_DB_FORMAT_H_
