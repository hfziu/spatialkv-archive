//
// Created by xiaofzhao4 on 12/15/2022.
//

#ifndef SMVKV_DB_FORMAT_H_
#define SMVKV_DB_FORMAT_H_

#include <string>

namespace smvkv {

using StreamID = uint32_t;
using SequenceNumber = uint64_t;
using ValidTime = uint64_t;

enum KeyType {
  kData = 0x01,
  kSpatial = 0x02,
  kTrajectory = 0x03,
};

struct Coordinate {
  double lat;
  double lon;
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

class DataKey : public Key {
 public:
  DataKey() = default;
  ~DataKey() override = default;

  [[nodiscard]] std::string Encode() const override;

  [[nodiscard]] std::string DebugString() const override;

 private:
  KeyType type_{kData};
  StreamID stream_id_{};
  SequenceNumber sequence_number_{};
  ValidTime valid_time_{};
};

class SpatialKey : public Key {
 public:
  SpatialKey() = default;
  ~SpatialKey() override = default;
};

}  // namespace smvkv

#endif  // SMVKV_DB_FORMAT_H_
