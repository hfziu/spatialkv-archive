//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "db/db_impl.h"

#include <memory>

#include "db_connector/connector.h"
#include "spatialkv/keyformat.h"
#include "values.pb.h"

namespace spatialkv {

SpatialKV::SpatialKV()
    : db_connector_(std::make_unique<LevelDBConnector>()),
      spatial_encoder_(S2CellIdEncoder()) {}

Status SpatialKV::Open(const std::string& db_path) {
  return db_connector_->Open(db_path);
}

Status SpatialKV::Put(const Slice& key, uint64_t time, uint64_t seq,
                      const Coordinate& coord, const Slice& value) {
  SpatialKey spatial_key{coord, spatial_encoder_};
  values::SpatialEntry entry;
  entry.set_trip_id(key.ToString());
  entry.set_time(time);
  entry.set_sequence(seq);
  entry.set_lat(coord.lat);
  entry.set_lng(coord.lng);
  entry.set_body(value.ToString());
  return db_connector_->Put(spatial_key.Encode(), entry.SerializeAsString());
}

// TODO: search adjacent cells to find the nearest point.
Status SpatialKV::GetSpatialPoint(const Coordinate& coord,
                                  ResultPointEntry* result, double distance) {
  SpatialKey spatial_key{coord, spatial_encoder_};
  std::string value;
  Status s = db_connector_->Get(spatial_key.Encode(), &value);
  if (!s.ok()) {
    return s;
  }
  values::SpatialEntry entry;
  entry.ParseFromString(value);
  result->FromPbSpatialEntry(entry);
  return Status::OK();
}

}  // namespace spatialkv