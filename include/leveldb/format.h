//
// Created by xiaofzhao4 on 9/25/2021.
//

#ifndef STORAGE_LEVELDB_INCLUDE_FORMAT_H_
#define STORAGE_LEVELDB_INCLUDE_FORMAT_H_

#include "leveldb/export.h"
#include <cstdint>

namespace leveldb {

using ValidTime = uint64_t;
static const ValidTime kMaxValidTime = 0xffffffffffffffff;

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_FORMAT_H_
