// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/memtable.h"

#include "db/dbformat.h"
#include "leveldb/comparator.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"
#include "util/coding.h"

namespace leveldb {

static Slice GetLengthPrefixedSlice(const char* data) {
  uint32_t len;
  const char* p = data;
  p = GetVarint32Ptr(p, p + 5, &len);  // +5: we assume "p" is not corrupted
  return {p, len};
}

MemTable::MemTable(const InternalKeyComparator& comparator, ValidTime vt,
                   spatial::Order n)
    : comparator_(comparator),
      refs_(0),
      table_(comparator_, &arena_),
      hilbert_(n),
      start_valid_time_(vt) {}

MemTable::~MemTable() { assert(refs_ == 0); }

size_t MemTable::ApproximateMemoryUsage() { return arena_.MemoryUsage(); }

int MemTable::KeyComparator::operator()(const char* aptr,
                                        const char* bptr) const {
  // Internal keys are encoded as length-prefixed strings.
  Slice a = GetLengthPrefixedSlice(aptr);
  Slice b = GetLengthPrefixedSlice(bptr);
  return comparator.Compare(a, b);
}

// int MemTable::SpatialIndexComparator::operator()(const char* a,
//                                                  const char* b) const {
//   spatial::Linear anum = DecodeFixed64(a);
//   spatial::Linear bnum = DecodeFixed64(b);
//   if (anum < bnum) {
//     return -1;
//   } else if (anum > bnum) {
//     return +1;
//   } else {
//     return 0;
//   }
// }

// Encode a suitable internal key target for "target" and return it.
// Uses *scratch as scratch space, and the returned pointer will point
// into this scratch space.
static const char* EncodeKey(std::string* scratch, const Slice& target) {
  scratch->clear();
  PutVarint32(scratch, target.size());
  scratch->append(target.data(), target.size());
  return scratch->data();
}

class MemTableIterator : public Iterator {
 public:
  explicit MemTableIterator(MemTable::Table* table) : iter_(table) {}

  MemTableIterator(const MemTableIterator&) = delete;
  MemTableIterator& operator=(const MemTableIterator&) = delete;

  ~MemTableIterator() override = default;

  bool Valid() const override { return iter_.Valid(); }
  void Seek(const Slice& k) override { iter_.Seek(EncodeKey(&tmp_, k)); }
  void SeekToFirst() override { iter_.SeekToFirst(); }
  void SeekToLast() override { iter_.SeekToLast(); }
  void Next() override { iter_.Next(); }
  void Prev() override { iter_.Prev(); }
  [[nodiscard]] Slice key() const override { return GetLengthPrefixedSlice(iter_.key()); }
  [[nodiscard]] Slice value() const override {
    Slice key_slice = GetLengthPrefixedSlice(iter_.key());
    return GetLengthPrefixedSlice(key_slice.data() + key_slice.size());
  }

  [[nodiscard]] Status status() const override { return Status::OK(); }

 private:
  MemTable::Table::Iterator iter_;
  std::string tmp_;  // For passing to EncodeKey
};

Iterator* MemTable::NewIterator() { return new MemTableIterator(&table_); }

void MemTable::Add(SequenceNumber s, ValueType type, const Slice& key,
                   const Slice& value) {
  // Format of an entry is concatenation of:
  //  key_size     : varint32 of internal_key.size()
  //  key bytes    : char[internal_key.size()]
  //  value_size   : varint32 of value.size()
  //  value bytes  : char[value.size()]
  size_t key_size = key.size();
  size_t val_size = value.size();
  size_t internal_key_size = key_size + 8;
  const size_t encoded_len = VarintLength(internal_key_size) +
                             internal_key_size + VarintLength(val_size) +
                             val_size;
  char* buf = arena_.Allocate(encoded_len);
  char* p = EncodeVarint32(buf, internal_key_size);
  std::memcpy(p, key.data(), key_size);
  p += key_size;
  EncodeFixed64(p, (s << 8) | type);
  p += 8;
  p = EncodeVarint32(p, val_size);
  std::memcpy(p, value.data(), val_size);
  assert(p + val_size == buf + encoded_len);
  table_.Insert(buf);
}

void MemTable::Add(SequenceNumber s, ValueType type, const Slice& key,
                   ValidTime vt, spatial::Linear x, spatial::Linear y,
                   const Slice& value) {
  // Format of an entry is concatenation of:
  // key_size      : varint32 of internal_key.size()
  // key bytes     : char[internal_key.size()]
  // valid_time    : fixed64
  // coordinate x  : fixed64
  // coordinate y  : fixed64
  // value_size    : varint32 of value.size()
  // value bytes   : char[value.size()]
  size_t key_size = key.size();
  size_t val_size = value.size();
  size_t internal_key_size =
      key_size + kInternalKeyAttributesLen;
  const size_t encoded_len =
      VarintLength(internal_key_size) + internal_key_size
      + VarintLength(val_size) + val_size;
  char* buf = arena_.Allocate(encoded_len);
  char* p = EncodeVarint32(buf, internal_key_size);
  std::memcpy(p, key.data(), key_size);
  p += key_size;
  EncodeFixed64(p, (s << 8) | type);
  p += 8;
  EncodeFixed64(p, vt);
  p += 8;
  EncodeFixed64(p, x);
  p += 8;
  EncodeFixed64(p, y);
  p += 8;
  p = EncodeVarint32(p, val_size);
  std::memcpy(p, value.data(), val_size);
  assert(p + val_size == buf + encoded_len);

  // Insert the data entry and return the address
  auto loc = reinterpret_cast<uint64_t>(table_.Insert(buf));
  char* encoded_loc = new char[8];
  EncodeFixed64(encoded_loc, loc);

  spatial::Linear t;
  hilbert_.MapInverse(x, y, &t);
  char* spatial_key = new char[8];
  EncodeFixed64(spatial_key, (0x1cull << 56) | t);  // default key length is 28

  if (spatial_table_.count(spatial_key)) {
    // Key exists
    spatial_table_[spatial_key].emplace_back(encoded_loc);
  } else {
    spatial_table_.emplace(spatial_key, IndexPointerBucket{encoded_loc});
  }
  //  char* spatial_buf = arena_.Allocate(16);
  //  EncodeFixed64(spatial_buf, t);
  //  EncodeFixed64(spatial_buf + 8, loc);
  //  spatial_table_.Insert(spatial_buf);
}

bool MemTable::Get(const LookupKey& key, std::string* value, Status* s) {
  Slice memkey = key.memtable_key();
  Table::Iterator iter(&table_);
  iter.Seek(memkey.data());
  if (iter.Valid()) {
    // entry format is:
    //    klength  varint32
    //    userkey  char[klength]
    //    tag      uint64
    //    vlength  varint32
    //    value    char[vlength]
    // Check that it belongs to same user key.  We do not check the
    // sequence number since the Seek() call above should have skipped
    // all entries with overly large sequence numbers.
    const char* entry = iter.key();
    uint32_t key_length;
    const char* key_ptr = GetVarint32Ptr(entry, entry + 5, &key_length);
    if (comparator_.comparator.user_comparator()->Compare(
            Slice(key_ptr, key_length - 8), key.user_key()) == 0) {
      // Correct user key
      const uint64_t tag = DecodeFixed64(key_ptr + key_length - 8);
      switch (static_cast<ValueType>(tag & 0xff)) {
        case kTypeValue: {
          Slice v = GetLengthPrefixedSlice(key_ptr + key_length);
          value->assign(v.data(), v.size());
          return true;
        }
        case kTypeDeletion:
          *s = Status::NotFound(Slice());
          return true;
      }
    }
  }
  return false;
}

bool MemTable::Get(const LookupKey& key, spatial::Linear x, spatial::Linear y,
                   std::string* value, spatial::Linear* res_x,
                   spatial::Linear* res_y, Status* s, int min_level) {
  // min_level:
  //    The result must match the search target in at least min_level levels.
  //    The default is 4.
  //    Example:
  Slice memkey = key.memtable_key();
  // Build spatial index target
  spatial::Linear i;
  char* spatial_idx = new char[8];
  hilbert_.MapInverse(x, y, &i);
  EncodeFixed64(spatial_idx, (((uint64_t)(min_level * 2)) << 56) | i);
  // Search spatial index
  std::pair<SpatialTable::const_iterator, SpatialTable::const_iterator> range;
  range = spatial_table_.equal_range(spatial_idx);
  // SpatialTable::const_iterator start = spatial_table_.lower_bound(spatial_idx);
  // SpatialTable::const_iterator end = spatial_table_.upper_bound(spatial_idx);
  if (range.first != spatial_table_.cend()) {
    // At least one result exists
    double distance = spatial::kMaxDistance;
    std::map<const double, const char*> results;
    for (auto& iter = range.first; iter != range.second; ++iter) {
      const IndexPointerBucket& bucket = iter->second;
      for (const auto& idx_entry : bucket) {
        auto p = reinterpret_cast<Table::Node*>(DecodeFixed64(idx_entry));
        const char* entry = p->key;
        // Check entry from base table
        uint32_t key_length;
        const char* key_ptr = GetVarint32Ptr(entry, entry + 5, &key_length);
        if (comparator_.comparator.user_comparator()->Compare(
                Slice(key_ptr, key_length - 16), key.user_key()) == 0) {
          // Correct user key
          const uint64_t tag = DecodeFixed64(key_ptr + key_length - 16);
          switch (static_cast<ValueType>(tag & 0xff)) {
            case kTypeValue: {
              // +16: Skip 16 bytes spatial coordinates
              Slice v = GetLengthPrefixedSlice(key_ptr + key_length + 16);
              value->assign(v.data(), v.size());
              return true;
            }
            case kTypeDeletion:
              *s = Status::NotFound(Slice());
              return true;
          }
          // TODO: this is incorrect.
        }
      }
    }
  }
  return false;
}

}  // namespace leveldb
