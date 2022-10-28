//
// Created by Xiaofei ZHAO on 4/6/2021.
//

#ifndef SPATIAL_LEVELDB_SPATIAL_CURVE_H_
#define SPATIAL_LEVELDB_SPATIAL_CURVE_H_

#include <bitset>
#include <string>
#include <vector>

#include "spatial/format.h"
#include "util/coding.h"

namespace spatial {

// spatial index format:
//   bitlength 8 bits

char* NextIdx(char* t);

struct SpatialBitComparator {
  // A custom comparator used for std::map
  // Returns true if a < b.
  explicit SpatialBitComparator() = default;
  bool operator()(const char *a, const char *b) const;

  static inline int LSB64(uint64_t n) {
    // least significant set bit (first bit's index = 0)
    int b = 0;
    if (!n) return -1;
    if (n >= 1ull << 63) return 0;

#define shift(x) \
  if (n < (1ull << (64 - x))) b += x, n <<= x
    shift(32);
    shift(16);
    shift(8);
    shift(4);
    shift(2);
    shift(1);
#undef shift  //(x)
    return b;
  }
};

class SpaceCurve {
 public:
  // Maps an integer on the Hilbert Curve to a 2-D coordinate.
  virtual bool Map(Linear t, Coordinate *coord) const = 0;

  // Maps a 2-D coordinate to an integer index on the Space Curve
  // Integer output.
  virtual bool MapInverse(const Coordinate &coord, uint64_t *t) const = 0;
  // String output (binary representation of t).
  virtual bool MapInverse(const Coordinate &coord, std::string *t) const = 0;

  // Returns the set of cells overlap with the rectangle (x1, y1) - (x2, y2)
  virtual bool Rectangle(const Coordinate &top_left,
                         const Coordinate &bottom_right,
                         std::vector<Linear> *cells) const = 0;

 private:
};

class Hilbert : public SpaceCurve {
 public:
  // REQUIRES: n is positive
  explicit Hilbert() = default;
  explicit Hilbert(Order n) : n_(n) {}
  Hilbert(Order n, Coord lat_min, Coord lat_max, Coord lon_min, Coord lon_max)
      : n_(n),
        latitude_min_(lat_min),
        latitude_max_(lat_max),
        longitude_min_(lon_min),
        longitude_max_(lon_max) {}

  bool Map(Linear t, Coordinate *coord) const override;
  bool Map(Linear t, uint64_t *lat, uint64_t *lon) const;

  // Coordinate input.
  bool MapInverse(const Coordinate &coord, uint64_t *t) const override;
  // Integers (two parameters x, y are indices of grid cells) input.
  bool MapInverse(Linear lat, Linear lon, uint64_t *t) const;
  // Output binary representation for t (as string).
  bool MapInverse(const Coordinate &coord, std::string *t) const override;
  bool MapInverse(Linear lat, Linear lon, std::string *t) const;

  bool Rectangle(const Coordinate &top_left, const Coordinate &bottom_right,
                 std::vector<Linear> *cells) const override;

  // Helper function for Mapping(Inverse) to/from Hilbert Curve
  // Input l is the length of a side of the square.
  // l must be a power of 2.
  static bool Rotate(Linear l, bool rx, bool ry, Linear &x, Linear &y);

  // Matches a float coordinate (lat, lon) to grid cell index (x, y)
  void MatchToCell(const Coordinate &coord, Linear &x, Linear &y) const;

 private:
  Order n_ {28};  // a maximum of 56 bits to store spatial index
  Coord latitude_min_{-90.0};
  Coord latitude_max_{90.0};
  Coord longitude_min_{-180.0};
  Coord longitude_max_{180.0};
};

class Geohash : public SpaceCurve {
 public:
  // REQUIRES: n is positive
  explicit Geohash(Order n) : n_(n) {}

  bool Map(Linear t, Coordinate *coord) const override;

  // Convert a 2-D coordinate to an uint64_t integer. Returns true on success,
  // and false on failure. The least significant (2 * n_) bits are valid data.
  bool MapInverse(const Coordinate &coord, uint64_t *t) const override;
  // Binary representation (as string).
  bool MapInverse(const Coordinate &coord, std::string *t) const override;

  bool Rectangle(const Coordinate &top_left, const Coordinate &bottiom_right,
                 std::vector<Linear> *cells) const override;

 private:
  Order n_ {28};  // a maximum of 56 bits to store spatial index
  Coord latitude_min_{-90.0};
  Coord latitude_max_{90.0};
  Coord longitude_min_{-180.0};
  Coord longitude_max_{180.0};
};

}  // namespace spatial

#endif  // SPATIAL_LEVELDB_SPATIAL_CURVE_H_
