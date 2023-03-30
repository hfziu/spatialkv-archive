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

 private:
   std::unique_ptr<Connector> db_connector_{};
   const SpatialEncoder* spatial_encoder_{};
};

}  // namespace spatialkv

#endif  // SPATIALKV_INCLUDE_SPATIALKV_DB_H_
