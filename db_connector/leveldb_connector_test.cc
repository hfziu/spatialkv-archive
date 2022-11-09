//
// Created by xiaofzhao4 on 10/28/2022.
//

#include "db_connector/connector.h"
#include "gtest/gtest.h"

namespace spatialkv {

TEST(Connector, Empty) { Connector connector; }

TEST(LevelDBConnector, Open) {
  LevelDBConnector connector;
  auto status = connector.Open("test_db");
  EXPECT_TRUE(status.ok());
}

TEST(LevelDBConnector, Put) {
  LevelDBConnector connector;
  auto status = connector.Open("test_db");
  EXPECT_TRUE(status.ok());
  status = connector.Put("key", "value");
  EXPECT_TRUE(status.ok());
}

TEST(LevelDBConnector, Get) {
  LevelDBConnector connector;
  auto status = connector.Open("test_db");
  EXPECT_TRUE(status.ok());
  std::string value;
  status = connector.Get("key", &value);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(value, "value");
}

}  // namespace spatialkv
