//
// Created by xiaofzhao4 on 12/15/2022.
//

#ifndef SMVKV_DB_FORMAT_H_
#define SMVKV_DB_FORMAT_H_

#include <string>
#include <utility>

#include "values.pb.h"

namespace spatialkv {

using TripID = std::string;
using SequenceNumber = uint64_t;
using ValidTime = uint64_t;
using Lat = double;
using Lng = double;

struct Coordinate {
  Lat lat;
  Lng lng;
};

class ResultPointEntry {
 public:
  ResultPointEntry() = default;

  // Copy constructor.
  ResultPointEntry(const ResultPointEntry& other) {
    trip_id_ = other.trip_id_;
    sequence_number_ = other.sequence_number_;
    valid_time_ = other.valid_time_;
    coordinate_ = other.coordinate_;
    value_ = other.value_;
  }
  // Move constructor.
  ResultPointEntry(ResultPointEntry&& other) noexcept {
    trip_id_ = std::move(other.trip_id_);
    sequence_number_ = other.sequence_number_;
    valid_time_ = other.valid_time_;
    coordinate_ = other.coordinate_;
    value_ = std::move(other.value_);
  }

  // Set values from protobuf message SpatialEntry.
  void FromPbSpatialEntry(const values::SpatialEntry& pb_msg) {
    trip_id_ = pb_msg.trip_id();
    sequence_number_ = pb_msg.sequence();
    valid_time_ = pb_msg.time();
    coordinate_ = {pb_msg.lat(), pb_msg.lng()};
    value_ = pb_msg.body();
  }

  // Set values from protobuf message TemporalEntry.
  void FromPbTemporalEntry(const values::TemporalEntry& pb_msg,
                           const TripID& trip_id, ValidTime time) {
    trip_id_ = trip_id;
    sequence_number_ = pb_msg.sequence();
    valid_time_ = time;
    coordinate_ = {pb_msg.lat(), pb_msg.lng()};
    value_ = pb_msg.body();
  }

  [[nodiscard]] const TripID& trip_id() const { return trip_id_; }
  [[nodiscard]] const SequenceNumber& sequence_number() const {
    return sequence_number_;
  }
  [[nodiscard]] const ValidTime& valid_time() const { return valid_time_; }
  [[nodiscard]] const Coordinate& coordinate() const { return coordinate_; }
  [[nodiscard]] const std::string& value() const { return value_; }

  [[nodiscard]] std::string DebugString() const {
    return "trip_id: " + trip_id_ +
           ", sequence_number: " + std::to_string(sequence_number_) +
           ", valid_time: " + std::to_string(valid_time_) + ", coordinate: (" +
           std::to_string(coordinate_.lat) + ", " +
           std::to_string(coordinate_.lng) + "), value: " + value_;
  }

 private:
  TripID trip_id_{};
  SequenceNumber sequence_number_{};
  ValidTime valid_time_{};
  Coordinate coordinate_{};
  std::string value_{};
};

}  // namespace spatialkv

#endif  // SMVKV_DB_FORMAT_H_
