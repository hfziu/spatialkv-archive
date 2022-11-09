//
// Created by xiaofzhao4 on 10/28/2022.
//

#ifndef SMVKV_DB_CONNECTOR_CONNECTOR_H_
#define SMVKV_DB_CONNECTOR_CONNECTOR_H_

#include <memory>

#include "leveldb/db.h"
#include "leveldb/options.h"

namespace smvkv {

using Status = leveldb::Status;

class Connector {
 public:
  Connector() = default;
  virtual ~Connector() = default;
};

class LevelDBConnector : public Connector {
 public:
  LevelDBConnector();
  ~LevelDBConnector() override;

  Status Open(const std::string& db_path);
  Status Put(const std::string& key, const std::string& value);
  Status Get(const std::string& key, std::string* value);

 private:
  leveldb::DB* db_{};
  leveldb::Options options_;
  leveldb::ReadOptions read_options_;
  leveldb::WriteOptions write_options_;
};

}  // namespace smvkv

#endif  // SMVKV_DB_CONNECTOR_CONNECTOR_H_
