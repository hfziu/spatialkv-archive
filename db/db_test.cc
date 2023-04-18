//
// Created by xiaofzhao4 on 3/30/2023.
//

#include "spatialkv/db.h"

#include <filesystem>
#include <string>

#include "gtest/gtest.h"
#include "spatialkv/dbformat.h"
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
    std::string key = std::to_string(random_int(0, 0xff));
    double lat = random_double(-90, 90);
    double lng = random_double(-180, 180);
    std::string value = std::to_string(random_int(0, 0xff));
    auto s_put = db.Put(key, i, i, {lat, lng}, value);
    EXPECT_TRUE(s_put.ok());

    // read the data entry
    auto result = db.GetSpatialPoint({lat, lng});
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->trip_id(), key);
    EXPECT_EQ(result->value(), value);
    if (i % 5 == 0) std::cout << result->DebugString() << std::endl;
  }

  fs::remove_all(db_path);
}

}  // namespace spatialkv