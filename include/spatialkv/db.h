//
// Created by xiaofzhao4 on 3/29/2023.
//

#ifndef SPATIALKV_INCLUDE_SPATIALKV_DB_H_
#define SPATIALKV_INCLUDE_SPATIALKV_DB_H_

#include <cstdint>
#include <string_view>

#include "db_connector/connector.h"
#include "leveldb/db.h"
#include "spatialkv/dbformat.h"
#include "util/encoder.h"

namespace spatialkv {

using Slice = leveldb::Slice;
using Status = leveldb::Status;
using Options = leveldb::Options;
using ReadOptions = leveldb::ReadOptions;
using WriteOptions = leveldb::WriteOptions;

class SpatialKV {
 public:
  SpatialKV();
  ~SpatialKV() = default;

  Status Open(const std::string& db_path);

  Status Put(const Slice& key, uint64_t time, uint64_t seq,
             const Coordinate& coord, const Slice& value);

  // Get a point that is within the distance (kilometers) of the given coordinate.
  Status GetSpatialPoint(const Coordinate& coord, ResultPointEntry* result,
                         double distance = 1.0);

  Status GetTemporalPoint(const TripID& trip_id, ValidTime time,
                          ResultPointEntry* result);

  // Compact the live entries of key into static trajectory segments.
  void Compact(const Slice& key);

 private:
  leveldb::DB* db_{};
  leveldb::Options options_{leveldb::Options()};
  leveldb::ReadOptions read_options_{leveldb::ReadOptions()};
  leveldb::WriteOptions write_options_{leveldb::WriteOptions()};
  const SpatialEncoder* spatial_encoder_{};
};

}  // namespace spatialkv

#endif  // SPATIALKV_INCLUDE_SPATIALKV_DB_H_
