// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_DBFORMAT_H_
#define STORAGE_LEVELDB_DB_DBFORMAT_H_

#include <cstddef>
#include <cstdint>
#include <string>

#include "leveldb/comparator.h"
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/format.h"
#include "leveldb/slice.h"
#include "leveldb/table_builder.h"
#include "util/coding.h"
#include "util/logging.h"

namespace leveldb {

// Grouping of constants.  We may want to make some of these
// parameters set via options.
namespace config {
static const int kNumLevels = 7;

// Level-0 compaction is started when we hit this many files.
// TODO: MVLevelDB: we do not compact L0 files.
static const int kL0_CompactionTrigger = 400000;

// Soft limit on number of level-0 files.  We slow down writes at this point.
static const int kL0_SlowdownWritesTrigger = 800000;

// Maximum number of level-0 files.  We stop writes at this point.
static const int kL0_StopWritesTrigger = 1200000;

// Maximum level to which a new compacted memtable is pushed if it
// does not create overlap.  We try to push to level 2 to avoid the
// relatively expensive level 0=>1 compactions and to avoid some
// expensive manifest file operations.  We do not push all the way to
// the largest level since that can generate a lot of wasted disk
// space if the same key space is being repeatedly overwritten.
static const int kMaxMemCompactLevel = 0;

// Approximate gap in bytes between samples of data read during iteration.
static const int kReadBytesPeriod = 1048576;

}  // namespace config

class InternalKey;

// Value types encoded as the last component of internal keys.
// DO NOT CHANGE THESE ENUM VALUES: they are embedded in the on-disk
// data structures.
enum ValueType { kTypeDeletion = 0x0, kTypeValue = 0x1 };
// kValueTypeForSeek defines the ValueType that should be passed when
// constructing a ParsedInternalKey object for seeking to a particular
// sequence number (since we sort sequence numbers in decreasing order
// and the value type is embedded as the low 8 bits in the sequence
// number in internal keys, we need to use the highest-numbered
// ValueType, not the lowest).
static const ValueType kValueTypeForSeek = kTypeValue;

typedef uint64_t SequenceNumber;

// We leave eight bits empty at the bottom so a type and sequence#
// can be packed together into 64-bits.
static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);

// An internal key is a concatenation of:
//   - user key
//   - tag: 1 byte
//   - sequence number: 7 bytes
//   - valid time: 8 bytes
//   - coordinates x, y: 16 bytes
static const int kInternalKeyAttributesLen = 1 + 7 + 8 + 16;

struct ParsedInternalKey {
  Slice user_key;
  SequenceNumber sequence{};
  ValueType type;
  ValidTime time{};
  spatial::Linear x{};
  spatial::Linear y{};

  ParsedInternalKey() = default;  // Intentionally left uninitialized (for speed)
  ParsedInternalKey(const Slice& u, const SequenceNumber& seq, ValueType t,
                    ValidTime vt, spatial::Linear x, spatial::Linear y)
      : user_key(u), sequence(seq), type(t), time(vt), x(x), y(y) {}
  [[nodiscard]] std::string DebugString() const;
};

// Return the length of the encoding of "key".
inline size_t InternalKeyEncodingLength(const ParsedInternalKey& key) {
  return key.user_key.size() + kInternalKeyAttributesLen;
}

// Append the serialization of "key" to *result.
void AppendInternalKey(std::string* result, const ParsedInternalKey& key);

// Attempt to parse an internal key from "internal_key".  On success,
// stores the parsed data in "*result", and returns true.
//
// On error, returns false, leaves "*result" in an undefined state.
bool ParseInternalKey(const Slice& internal_key, ParsedInternalKey* result);

// Returns the user key portion of an internal key.
inline Slice ExtractUserKey(const Slice& internal_key) {
  assert(internal_key.size() >= kInternalKeyAttributesLen);
  return {internal_key.data(), internal_key.size() - kInternalKeyAttributesLen};
}

inline uint64_t ExtractFixed64(const Slice& fixed64) {
  assert(fixed64.size() == 8);
  return DecodeFixed64(fixed64.data());
}

// A comparator for internal keys that uses a specified comparator for
// the user key portion and breaks ties by decreasing sequence number.
class InternalKeyComparator : public Comparator {
 private:
  const Comparator* user_comparator_;

 public:
  explicit InternalKeyComparator(const Comparator* c) : user_comparator_(c) {}
  const char* Name() const override;
  int Compare(const Slice& a, const Slice& b) const override;
  void FindShortestSeparator(std::string* start,
                             const Slice& limit) const override;
  void FindShortSuccessor(std::string* key) const override;

  const Comparator* user_comparator() const { return user_comparator_; }

  int Compare(const InternalKey& a, const InternalKey& b) const;
};

// Filter policy wrapper that converts from internal keys to user keys
// TODO: update to support multi-version and spatial attributes
class InternalFilterPolicy : public FilterPolicy {
 private:
  const FilterPolicy* const user_policy_;

 public:
  explicit InternalFilterPolicy(const FilterPolicy* p) : user_policy_(p) {}
  [[nodiscard]] const char* Name() const override;
  void CreateFilter(const Slice* keys, int n, std::string* dst) const override;
  [[nodiscard]] bool KeyMayMatch(const Slice& key,
                                 const Slice& filter) const override;
};

// Modules in this directory should keep internal keys wrapped inside
// the following class instead of plain strings so that we do not
// incorrectly use string comparisons instead of an InternalKeyComparator.
class InternalKey {
 private:
  std::string rep_;

 public:
  InternalKey() = default;  // Leave rep_ as empty to indicate it is invalid
  InternalKey(const Slice& user_key, SequenceNumber s, ValueType t,
              ValidTime vt, spatial::Linear x, spatial::Linear y) {
    AppendInternalKey(&rep_, ParsedInternalKey(user_key, s, t, vt, x, y));
  }

  bool DecodeFrom(const Slice& s) {
    rep_.assign(s.data(), s.size());
    return !rep_.empty();
  }

  [[nodiscard]] Slice Encode() const {
    assert(!rep_.empty());
    return rep_;
  }

  [[nodiscard]] Slice user_key() const { return ExtractUserKey(rep_); }

  [[nodiscard]] ValidTime valid_time() const {
    return DecodeFixed64(rep_.data() + rep_.size() - kInternalKeyAttributesLen +
                         8);
  }
  [[nodiscard]] spatial::Linear x() const {
    return DecodeFixed64(rep_.data() + rep_.size() - kInternalKeyAttributesLen +
                         16);
  }

  [[nodiscard]] spatial::Linear y() const {
    return DecodeFixed64(rep_.data() + rep_.size() - kInternalKeyAttributesLen +
                         24);
  }

  void SetFrom(const ParsedInternalKey& p) {
    rep_.clear();
    AppendInternalKey(&rep_, p);
  }

  void Clear() { rep_.clear(); }

  [[nodiscard]] std::string DebugString() const;
};

inline int InternalKeyComparator::Compare(const InternalKey& a,
                                          const InternalKey& b) const {
  return Compare(a.Encode(), b.Encode());
}

inline bool ParseInternalKey(const Slice& internal_key,
                             ParsedInternalKey* result) {
  const size_t n = internal_key.size();
  if (n < kInternalKeyAttributesLen) return false;
  result->time = static_cast<ValidTime>(
      DecodeFixed64(internal_key.data() + n - kInternalKeyAttributesLen + 8));
  uint64_t num =
      DecodeFixed64(internal_key.data() + n - kInternalKeyAttributesLen);
  uint8_t c = num & 0xff;
  result->sequence = num >> 8;
  result->type = static_cast<ValueType>(c);
  result->x = static_cast<spatial::Linear>(
      DecodeFixed64(internal_key.data() + n - kInternalKeyAttributesLen + 16));
  result->y = static_cast<spatial::Linear>(
      DecodeFixed64(internal_key.data() + n - kInternalKeyAttributesLen + 24));
  result->user_key = Slice(internal_key.data(), n - kInternalKeyAttributesLen);
  return (c <= static_cast<uint8_t>(kTypeValue));
}

// A helper class useful for DBImpl::Get()
class LookupKey {
 public:
  // Initialize *this for looking up user_key at a snapshot with
  // the specified sequence number.
  LookupKey(const Slice& user_key, SequenceNumber sequence, ValidTime vt);

  LookupKey(const LookupKey&) = delete;
  LookupKey& operator=(const LookupKey&) = delete;

  ~LookupKey();

  // Return a key suitable for lookup in a MemTable.
  [[nodiscard]] Slice memtable_key() const {
    return {start_, static_cast<size_t>(end_ - start_)};
  }

  // Return an internal key (suitable for passing to an internal iterator)
  [[nodiscard]] Slice internal_key() const {
    return {kstart_, static_cast<size_t>(end_ - kstart_)};
  }

  // Return the user key
  [[nodiscard]] Slice user_key() const {
    return {kstart_, static_cast<size_t>(vtstart_ - kstart_ - 8)};
  }

  [[nodiscard]] ValidTime valid_time() const {
    return DecodeFixed64(vtstart_);
  }

 private:
  // We construct a char array of the form:
  //    klength  varint32               <-- start_
  //    userkey  char[klength]          <-- kstart_
  //    tag      uint64
  //    vt       uint64                 <-- vtstart_
  //                                    <-- end_
  // The array is a suitable MemTable key.
  // The suffix starting with "userkey" can be used as an InternalKey.
  const char* start_;
  const char* kstart_;
  const char* vtstart_;  // start of valid time field
  const char* end_;
  char space_[200];  // Avoid allocation for short keys
};

inline LookupKey::~LookupKey() {
  if (start_ != space_) delete[] start_;
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_DBFORMAT_H_
