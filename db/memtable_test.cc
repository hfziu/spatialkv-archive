//
// Created by xiaofzhao4 on 10/10/2021.
//

#include "db/memtable.h"
#include "db/dbformat.h"
#include "spatial/format.h"

#include "db/write_batch_internal.h"
#include "gtest/gtest.h"
#include "leveldb/env.h"

namespace leveldb {

InternalKeyComparator cmp(BytewiseComparator());

//TEST(MemTableTest, SimpleSpatial) {
//  MemTable* mem = new MemTable(cmp, 0, 4);
//  mem->Ref();
//  mem->Add(1, kTypeValue, Slice("k1"), 1, 8, 12, Slice("v1"));
//  mem->Add(2, kTypeValue, Slice("k2"), 2, 8, 14, Slice("v2"));
//  mem->Add(3, kTypeValue, Slice("k3"), 3, 4, 9, Slice("v3"));
//  std::string value;
//  Status s;
//  LookupKey lk1 = LookupKey("k1", 1, 1);
//  ASSERT_TRUE(mem->Get(lk1, 8, 12, &value, &s));
//  ASSERT_EQ(value, "v1");
//  LookupKey lk2 = LookupKey("k2", 2, 2);
//  ASSERT_TRUE(mem->Get(lk2, 8, 14, &value, &s));
//  ASSERT_EQ(value, "v2");
//  LookupKey lk3 = LookupKey("k3", 3, 3);
//  ASSERT_TRUE(mem->Get(lk3, 4, 9, &value, &s));
//  ASSERT_EQ(value, "v3");
//  mem->Unref();
//}

}  // namespace leveldb

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}