//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "spatialkv/keyformat.h"

#include "util/coding.h"

namespace spatialkv {

std::string SpatialKey::Encode() const {
  std::string result = std::move(std::to_string(type_));
  result.append(
      BigEndianEncoder::EncodeFixed64(spatial_encoder_->Encode(coordinate_)));
  return result;
}

std::string SpatialKey::DebugString() const {
  return "SpatialKey:(" + std::to_string(coordinate_.lat) + "," +
         std::to_string(coordinate_.lng) + ")";
}

std::string TemporalKey::Encode() const {
  std::string result = std::move(std::to_string(type_));
  // stream_id_ is encoded as a length-prefixed string
  result.append(BigEndianEncoder::EncodeFixed64(stream_id_.size()));
  result.append(stream_id_);
  result.append(BigEndianEncoder::EncodeFixed64(sequence_number_));
  result.append(BigEndianEncoder::EncodeFixed64(valid_time_));
  // raw location without spatial encoding
  result.append(EncodeDouble(coordinate_.lat));
  result.append(EncodeDouble(coordinate_.lng));
  return result;
}

std::string TemporalKey::DebugString() const {
  return "TemporalKey:(" + stream_id_ + "," + std::to_string(sequence_number_) +
         "," + std::to_string(valid_time_) + "," +
         std::to_string(coordinate_.lat) + "," +
         std::to_string(coordinate_.lng) + ")";
}

std::string TemporalLookupKey::Encode() const {
  std::string result = std::move(std::to_string(type_));
  // stream_id_ is encoded as a length-prefixed string
  result.append(BigEndianEncoder::EncodeFixed64(stream_id_.size()));
  result.append(stream_id_);
  result.append(BigEndianEncoder::EncodeFixed64(valid_time_));
  return result;
}

std::string TemporalLookupKey::DebugString() const {
  return "TemporalLookupKey:(" + stream_id_ + "," +
         std::to_string(valid_time_) + ")";
}

}  // namespace spatialkv