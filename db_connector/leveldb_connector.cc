//
// Created by xiaofzhao4 on 10/28/2022.
//

#include <iostream>
#include "db_connector/connector.h"

namespace spatialkv {

LevelDBConnector::~LevelDBConnector() { delete db_; }

LevelDBConnector::LevelDBConnector() {
  options_.create_if_missing = true;
}

Status LevelDBConnector::Open(const optional<std::string>& db_path) {
  if (db_path) {
    return leveldb::DB::Open(options_, *db_path, &db_);
  } else if (db_path_) {
    return leveldb::DB::Open(options_, *db_path_, &db_);
  } else {
    return Status::InvalidArgument("db_path is not set");
  }
}

Status LevelDBConnector::Put(const Slice& key, const Slice& value) {
  return db_->Put(write_options_, key, value);

}

Status LevelDBConnector::Get(const Slice& key, std::string* value) {
  return db_->Get(read_options_, key, value);
}

}  // namespace spatialkv