//
// Created by xiaofzhao4 on 10/28/2022.
//

#ifndef SMVKV_DB_CONNECTOR_CONNECTOR_H_
#define SMVKV_DB_CONNECTOR_CONNECTOR_H_

#include <memory>

#include "leveldb/db.h"
#include "leveldb/options.h"

namespace spatialkv {

using Slice = leveldb::Slice;
using Status = leveldb::Status;

class Connector {
 public:
  Connector() = default;
  virtual ~Connector() = default;

  virtual Status Open(const std::string& db_path) = 0;
  virtual Status Put(const Slice& key, const Slice& value) = 0;
  virtual Status Get(const Slice& key, std::string* value) = 0;
};

class LevelDBConnector : public Connector {
 public:
  LevelDBConnector();
  ~LevelDBConnector() override;

  Status Open(const std::string& db_path) override;
  Status Put(const Slice& key, const Slice& value) override;
  Status Get(const Slice& key, std::string* value) override;

 private:
  leveldb::DB* db_{};
  leveldb::Options options_{leveldb::Options()};
  leveldb::ReadOptions read_options_{leveldb::ReadOptions()};
  leveldb::WriteOptions write_options_{leveldb::WriteOptions()};
};

}  // namespace spatialkv

#endif  // SMVKV_DB_CONNECTOR_CONNECTOR_H_
