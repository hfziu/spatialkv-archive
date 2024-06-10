//
// Created by Xiaofei ZHAO on 4/6/2021.
//

#include "curve.h"

#include <bitset>
#include <cassert>
#include <cmath>

namespace spatial {

char* NextIdx(char* t) {
  char* buffer = new char[8];
  uint64_t tnum = leveldb::DecodeFixed64(t);
  auto length = (tnum >> 56) & 0xff;
  leveldb::EncodeFixed64(buffer, tnum + (1ull << (56 - length)));
  return buffer;
}

bool SpatialBitComparator::operator()(const char *a, const char *b) const {
  auto anum = leveldb::DecodeFixed64(a);
  auto bnum = leveldb::DecodeFixed64(b);
  auto a_length = (anum >> 56) & 0xff;
  auto b_length = (bnum >> 56) & 0xff;
  uint8_t min_length = a_length < b_length ? a_length : b_length;
  uint64_t diff = (anum << 8) ^ (bnum << 8);
  int lsb = LSB64(diff) + 1;  // convert to 1-based index

  if (lsb <= min_length) {
    return (anum << 8) < (bnum << 8);
  }
  else {
    // TODO: this branch is not necessary if inputs satisfy the length assumption
    return a_length < b_length;
  }
}

bool Hilbert::Map(Linear t, Coordinate *coord) const {
  assert(0 <= t && t < (1ull << 2 * n_));
  // TODO: Map Hilbert Space Filling Curve
  return false;
}

bool Hilbert::Map(Linear t, uint64_t *lat, uint64_t *lon) const {
  Linear x = 0, y = 0;
  for (Order l = 1; l < (1ull << n_); l <<= 1) {
    bool ry = (t >> 1) & 1;
    bool rx = (t & 1) ^ ry;

    // l > x && l > y when rotate.
    Rotate(l, rx, ry, x, y);

    y += l * ry;
    x += l * rx;
    t >>= 2;
  }
  *lat = x;
  *lon = y;
  return true;
}

// REQUIRES: this->latitude_min_ <= coord.x < this->latitude_max_
//           this->longitude_min_ <= coord.y < this->longitude_max_
bool Hilbert::MapInverse(const Coordinate &coord, uint64_t *t) const {
  Linear x, y;
  MatchToCell(coord, x, y);
  return MapInverse(x, y, t);
}

// REQUIRES: lat < (1 << n_) && lon < (1 << n_)
bool Hilbert::MapInverse(Linear lat, Linear lon, uint64_t *t) const {
  // The length of a side of the square
  Linear l = 1ull << n_;

  Linear x = lat, y = lon;
  if (x >= l || y >= l) return false;  // Out of range

  l >>= 1;
  Linear index = 0;
  while (l > 0) {
    // Longitude: y
    bool ry = (y & l) > 0;
    // Latitude: x
    bool rx = (x & l) > 0;
    index <<= 2;
    index += (int(ry) << 1) + (int(ry) ^ int(rx));
    Rotate(l, rx, ry, x, y);
    l >>= 1;
  }
  *t = index;
  return true;
}

bool Hilbert::MapInverse(const Coordinate &coord, std::string *t) const {
  Linear index;
  if (MapInverse(coord, &index)) {
    const Order bits = 2 * n_;
    // Bitset size must consistent with the sizeof(Linear) * 8
    std::bitset<64> b(index);
    std::string bitstring = b.to_string().substr(64 - bits, bits);
    t->assign(bitstring);
    return true;
  }
  return false;
}

bool Hilbert::MapInverse(Linear lat, Linear lon, std::string *t) const {
  Linear index;
  if (MapInverse(lat, lon, &index)) {
    const Order bits = 2 * n_;
    // Bitset size must consistent with the sizeof(Linear) * 8
    std::bitset<64> b(index);
    std::string bitstring = b.to_string().substr(64 - bits, bits);
    t->assign(bitstring);
    return true;
  }
  return false;
}

bool Hilbert::Rotate(Order l, bool rx, bool ry, Order &x, Order &y) {
  if (!rx) {
    if (ry) {
      x = l - 1 - x;
      y = l - 1 - y;
    }
    std::swap(x, y);
  }
  return true;
}

bool Hilbert::Rectangle(const Coordinate &top_left,
                        const Coordinate &bottom_right,
                        std::vector<Linear> *cells) const {
  // TODO:
  return false;
}

void Hilbert::MatchToCell(const Coordinate &coord, Linear &x, Linear &y) const {
  // The length of a side of the square
  Order l = 1ull << n_;
  // The 2-D integer index of grid cell (x, y)
  x = static_cast<int>(floor(double(l) * (coord.x - latitude_min_) /
                             (latitude_max_ - latitude_min_)));
  y = static_cast<int>(floor(double(l) * (coord.y - longitude_min_) /
                             (longitude_max_ - longitude_min_)));
}

bool Geohash::Map(Linear t, Coordinate *coord) const {
  Order bits = 2 * n_;
  bool even = true;  // Shift calculation between longitude and latitude

  Coord lon_lo = longitude_min_, lon_hi = longitude_max_;
  Coord lat_lo = latitude_min_, lat_hi = latitude_max_;

  while (bits) {
    Order bit = t >> (bits - 1) & 1;
    if (even) {
      // longitude
      Coord lon_mi = (lon_lo + lon_hi) / 2;
      if (bit) {
        lon_lo = lon_mi;
      } else {
        lon_hi = lon_mi;
      }
    } else {
      // latitude
      Coord lat_mi = (lat_lo + lat_hi) / 2;
      if (bit) {
        lat_lo = lat_mi;
      } else {
        lat_hi = lat_mi;
      }
    }
    even = !even;
    bits--;
  }
  coord->x = (lat_lo + lat_hi) / 2;
  coord->y = (lon_lo + lon_hi) / 2;
  return true;
}

bool Geohash::MapInverse(const Coordinate &coord, Linear *t) const {
  Linear index = 0;
  Order bits = 0;
  bool even = true;  // Shift calculation between longitude and latitude

  Coord lon_lo = longitude_min_, lon_hi = longitude_max_;
  Coord lat_lo = latitude_min_, lat_hi = latitude_max_;

  while (bits < 2 * n_) {
    if (even) {
      // Longitude
      Coord lon_mi = (lon_lo + lon_hi) / 2;
      index <<= 1;  // add bit
      if (coord.y < lon_mi) {
        lon_hi = lon_mi;
      } else {
        index += 1;
        lon_lo = lon_mi;
      }
    } else {
      // Latitude
      Coord lat_mi = (lat_lo + lat_hi) / 2;
      index <<= 1;  // add bit
      if (coord.x < lat_mi) {
        lat_hi = lat_mi;
      } else {
        index += 1;
        lat_lo = lat_mi;
      }
    }
    even = !even;
    bits++;
  }
  *t = index;
  return true;
}

bool Geohash::MapInverse(const Coordinate &coord, std::string *t) const {
  Linear index;
  if (MapInverse(coord, &index)) {
    const Order bits = 2 * n_;
    // Bitset size must consistent with the sizeof(Linear) * 8
    std::bitset<64> b(index);
    std::string bitstring = b.to_string().substr(64 - bits, bits);
    t->assign(bitstring);
    return true;
  }
  return false;
}

bool Geohash::Rectangle(const Coordinate &top_left,
                        const Coordinate &bottiom_right,
                        std::vector<Linear> *cells) const {
  // TODO
  return false;
}

}  // namespace spatial