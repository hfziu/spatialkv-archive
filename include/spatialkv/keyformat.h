//
// Created by xiaofzhao4 on 3/29/2023.
//

#ifndef SPATIALKV_SPATIALKV_KEYFORMAT_H_
#define SPATIALKV_SPATIALKV_KEYFORMAT_H_

#include "util/encoder.h"

namespace spatialkv {

enum KeyType {
  kTemporal = 0x01,
  kSpatial = 0x02,
  kTrajectory = 0x03,
};

class Key {
 public:
  Key() = default;
  virtual ~Key() = default;

  // Returns the encoded key.
  [[nodiscard]] virtual std::string Encode() const = 0;

  [[maybe_unused]] [[nodiscard]] virtual std::string DebugString() const {
    return "Key: base key template";
  }
};

class TemporalKey : public Key {
 public:
  TemporalKey() = default;
  ~TemporalKey() override = default;

  [[nodiscard]] std::string Encode() const override;

  [[maybe_unused]] [[nodiscard]] std::string DebugString() const override;

 private:
  KeyType type_{kTemporal};
  StreamID stream_id_{};
  SequenceNumber sequence_number_{};
  ValidTime valid_time_{};
  Coordinate coordinate_{};  // raw location
};

class SpatialKey : public Key {
 public:
  SpatialKey() = default;
  ~SpatialKey() override = default;

  SpatialKey(const StreamID id, const ValidTime t, const SequenceNumber seq,
             const Coordinate& coordinate, SpatialEncoder* encoder)
      : stream_id_(id),
        valid_time_(t),
        sequence_number_(seq),
        coordinate_(coordinate),
        spatial_encoder_(encoder) {}

  [[nodiscard]] std::string Encode() const override;

  [[maybe_unused]] [[nodiscard]] std::string DebugString() const override;

 private:
  KeyType type_{kSpatial};
  StreamID stream_id_{};
  SequenceNumber sequence_number_{};
  ValidTime valid_time_{};
  Coordinate coordinate_{};  // raw location

  SpatialEncoder* spatial_encoder_{};

};

}  // namespace spatialkv

#endif  // SPATIALKV_SPATIALKV_KEYFORMAT_H_