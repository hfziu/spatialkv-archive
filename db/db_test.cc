//
// Created by xiaofzhao4 on 3/30/2023.
//

#include "spatialkv/db.h"

#include <filesystem>
#include <string>

#include "gtest/gtest.h"
#include "util/random.h"

namespace fs = std::filesystem;

std::string db_path = "test_db";

namespace spatialkv {

TEST(SpatialKV, Open) {
  SpatialKV db;
  auto status = db.Open(db_path);
  EXPECT_TRUE(status.ok());
  fs::remove_all(db_path);
}

TEST(SpatialKV, Put) {
  SpatialKV db;
  auto status = db.Open(db_path);
  EXPECT_TRUE(status.ok());

  RandomInt random_int;
  RandomDouble random_double;

  // feed random data
  for (int i = 0; i < 0xff; ++i) {
    db.Put(std::to_string(random_int(0, 0xff)), i, i,
           {random_double(-90, 90), random_double(-180, 180)},
           std::to_string(random_int(0, 0xff)));
  }

  fs::remove_all(db_path);
}

}  // namespace spatialkv