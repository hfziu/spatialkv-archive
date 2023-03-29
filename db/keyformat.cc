//
// Created by xiaofzhao4 on 3/29/2023.
//

#include "spatialkv/keyformat.h"

#include "keys.pb.h"

namespace spatialkv {

std::string SpatialKey::Encode() const {
  auto skey = std::make_unique<keys::SpatialKey>();
  skey->set_type(type_);
  skey->set_id(stream_id_);
  skey->set_seq(sequence_number_);
  skey->set_time(valid_time_);
  skey->set_location(spatial_encoder_->Encode(coordinate_));
  std::string encoded;
  skey->SerializeToString(&encoded);
  return encoded;
}

}  // namespace spatialkv