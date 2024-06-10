//
// Created by xiaofzhao4 on 10/14/2021.
//

#ifndef SPATIAL_LEVELDB_SPATIAL_INDEX_H_
#define SPATIAL_LEVELDB_SPATIAL_INDEX_H_

#include <map>
#include <vector>

#include "spatial/curve.h"

// implementation of Extendible Hashing Table.

namespace spatial {

class HashTable {
 public:
  explicit HashTable() = default;
  
 private:
  std::map<char*, std::vector<uint64_t>> table;
};

}  // namespace spatial

#endif  // SPATIAL_LEVELDB_SPATIAL_INDEX_H_
