//
// Created by xiaofzhao4 on 22/7/2021.
//

#ifndef STORAGE_LEVELDB_INCLUDE_SPATIAL_FORMAT_H_
#define STORAGE_LEVELDB_INCLUDE_SPATIAL_FORMAT_H_

#include <cstdint>
#include <limits>

namespace spatial {

using Coord = double;
using Linear = uint64_t;
using Order = uint64_t;

static const double kMaxDistance = std::numeric_limits<double>::max();

struct Coordinate {
  Coordinate() : x(0), y(0) {}
  Coordinate(Coord xx, Coord yy) : x(xx), y(yy) {}
  Coord x;
  Coord y;
};

// Drop the spatial attributes in internal keys
// TODO: use explicit tags
static const Linear kOmitCoordinate = std::numeric_limits<uint64_t>::max();

}  // namespace spatial

#endif  // STORAGE_LEVELDB_INCLUDE_SPATIAL_FORMAT_H_
