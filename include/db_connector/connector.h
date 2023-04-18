//
// Created by xiaofzhao4 on 10/28/2022.
//

#ifndef SMVKV_DB_CONNECTOR_CONNECTOR_H_
#define SMVKV_DB_CONNECTOR_CONNECTOR_H_

#include <memory>
#include <optional>
#include <string>

#include "leveldb/db.h"
#include "leveldb/options.h"

using std::optional;

namespace spatialkv {

using Slice = leveldb::Slice;
using Status = leveldb::Status;

class Connector {
 public:
  Connector() = default;
  explicit Connector(const std::string& db_path) : db_path_(db_path) {}
  virtual ~Connector() = default;

  virtual Status Open(const optional<std::string>& db_path) = 0;
  virtual Status Put(const Slice& key, const Slice& value) = 0;
  virtual Status Get(const Slice& key, std::string* value) = 0;
 protected:
  optional<std::string> db_path_;
};

class LevelDBConnector : public Connector {
 public:
  LevelDBConnector();
  ~LevelDBConnector() override;

  Status Open(const optional<std::string>& db_path) override;
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
