//
// Created by xiaofzhao4 on 10/28/2022.
//

#include <leveldb/options.h>

#include <filesystem>

#include "db_connector/connector.h"
#include "gtest/gtest.h"
#include "leveldb/db.h"

namespace fs = std::filesystem;

namespace spatialkv {

std::string db_path = "test_db";

TEST(LevelDB, Put) {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  auto status = leveldb::DB::Open(options, db_path, &db);
  EXPECT_TRUE(status.ok());
  // feed random data
  for (int i = 0; i < 0xff; ++i) {
    status = db->Put(leveldb::WriteOptions(), "key", "value");
  }
  EXPECT_TRUE(status.ok());
  delete db;
  fs::remove_all(db_path);
}

TEST(LevelDBConnector, Open) {
  LevelDBConnector connector;
  auto status = connector.Open(db_path);
  EXPECT_TRUE(status.ok());
}

TEST(LevelDBConnector, Put) {
  LevelDBConnector connector;
  auto status = connector.Open(db_path);
  EXPECT_TRUE(status.ok());
  // feed random data
  for (int i = 0; i < 0xff; ++i) {
    status = connector.Put("key", "value");
  }
  EXPECT_TRUE(status.ok());
}

TEST(LevelDBConnector, Get) {
  LevelDBConnector connector;
  auto status = connector.Open(db_path);
  EXPECT_TRUE(status.ok());
  std::string value;
  status = connector.Get("key", &value);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(value, "value");
  fs::remove_all(db_path);
}

}  // namespace spatialkv
