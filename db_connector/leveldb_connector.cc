//
// Created by xiaofzhao4 on 10/28/2022.
//

#include <iostream>
#include "db_connector/connector.h"

namespace smvkv {

LevelDBConnector::~LevelDBConnector() { delete db_; }

LevelDBConnector::LevelDBConnector() {
  options_.create_if_missing = true;
}

Status LevelDBConnector::Open(const std::string& db_path) {
  return leveldb::DB::Open(options_, db_path, &db_);
}

Status LevelDBConnector::Put(const std::string& key, const std::string& value) {
  return db_->Put(write_options_, key, value);

}

Status LevelDBConnector::Get(const std::string& key, std::string* value) {
  return db_->Get(read_options_, key, value);
}

}  // namespace smvkv