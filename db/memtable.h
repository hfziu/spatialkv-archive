// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_MEMTABLE_H_
#define STORAGE_LEVELDB_DB_MEMTABLE_H_

#include <cmath>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "db/dbformat.h"
#include "db/skiplist.h"
#include "leveldb/db.h"
#include "leveldb/format.h"
#include "spatial/curve.h"
#include "spatial/format.h"
#include "util/arena.h"

namespace leveldb {

class InternalKeyComparator;
class MemTableIterator;

class MemTable {
 public:
  // MemTables are reference counted.  The initial reference count
  // is zero and the caller must call Ref() at least once.
  explicit MemTable(const InternalKeyComparator& comparator, ValidTime vt,
                    spatial::Order n = 28);

  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  // Increase reference count.
  void Ref() { ++refs_; }

  // Drop reference count.  Delete if no more references exist.
  void Unref() {
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      delete this;
    }
  }

  // Returns an estimate of the number of bytes of data in use by this
  // data structure. It is safe to call when MemTable is being modified.
  size_t ApproximateMemoryUsage();

  // Return an iterator that yields the contents of the memtable.
  //
  // The caller must ensure that the underlying MemTable remains live
  // while the returned iterator is live.  The keys returned by this
  // iterator are internal keys encoded by AppendInternalKey in the
  // db/format.{h,cc} module.
  Iterator* NewIterator();

  // Add an entry into memtable that maps key to value at the
  // specified sequence number and with the specified type.
  // Typically value will be empty if type==kTypeDeletion.
  void Add(SequenceNumber seq, ValueType type, const Slice& key,
           const Slice& value);
  void Add(SequenceNumber seq, ValueType type, const Slice& key, ValidTime vt,
           spatial::Linear x, spatial::Linear y, const Slice& value);

  // If memtable contains a value for key, store it in *value and return true.
  // If memtable contains a deletion for key, store a NotFound() error
  // in *status and return true.
  // Else, return false.
  bool Get(const LookupKey& key, std::string* value, Status* s);
  bool Get(const LookupKey& key, spatial::Linear x, spatial::Linear y,
           std::string* value, spatial::Linear* res_x, spatial::Linear* res_y,
           Status* s, int min_level = 4);

  void SetStartValidTime(ValidTime t) { start_valid_time_ = t; }
  void SetEndValidTime(ValidTime t) { end_valid_time_ = t; }
  ValidTime GetStartValidTime() const { return start_valid_time_; }
  ValidTime GetEndValidTime() const { return end_valid_time_; }

 private:
  friend class MemTableIterator;
  friend class MemTableBackwardIterator;

  struct KeyComparator {
    const InternalKeyComparator comparator;
    explicit KeyComparator(InternalKeyComparator  c) : comparator(std::move(c)) {}
    int operator()(const char* a, const char* b) const;
  };

//  struct SpatialIndexComparator {
//    explicit SpatialIndexComparator() = default;
//    int operator()(const char* a, const char* b) const;
//  };

  typedef SkipList<const char*, KeyComparator> Table;

  // typedef SkipList<const char*, SpatialIndexComparator> SpatialTable;
  using IndexPointerBucket = std::vector<char*>;
  using SpatialTable =
      std::map<char*, IndexPointerBucket, spatial::SpatialBitComparator>;

  ~MemTable();  // Private since only Unref() should be used to delete it

  KeyComparator comparator_;
//  SpatialIndexComparator spatial_comparator_;
  int refs_;
  Arena arena_;
  Table table_;
  SpatialTable spatial_table_;
  spatial::Hilbert hilbert_;  // default n = 28

  ValidTime start_valid_time_;
  ValidTime end_valid_time_{};
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_MEMTABLE_H_
