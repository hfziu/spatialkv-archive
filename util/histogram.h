// Adapted from leveldb/util/histogram.h:

// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_HISTOGRAM_H_
#define STORAGE_LEVELDB_UTIL_HISTOGRAM_H_

#include <string>

namespace spatialkv {

class Histogram {
 public:
  Histogram() {}
  ~Histogram() = default;

  void Clear();
  void Add(double value);
  void Merge(const Histogram& other);

  [[nodiscard]] std::string ToString() const;

 private:
  enum { kNumBuckets = 154 };

  [[nodiscard]] double Median() const;
  [[nodiscard]] double Percentile(double p) const;
  [[nodiscard]] double Average() const;
  [[nodiscard]] double StandardDeviation() const;

  static const double kBucketLimit[kNumBuckets];

  double min_{};
  double max_{};
  double num_{};
  double sum_{};
  double sum_squares_{};

  double buckets_[kNumBuckets]{};
};

}  // namespace spatialkv

#endif  // STORAGE_LEVELDB_UTIL_HISTOGRAM_H_
