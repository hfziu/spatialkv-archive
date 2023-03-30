// A customized random generator for convenient testing.

#ifndef SPATIALKV_UTIL_RANDOM_H_
#define SPATIALKV_UTIL_RANDOM_H_

#include <random>

namespace spatialkv {

class Random {
 public:
  Random() = default;
  ~Random() = default;

 protected:
  std::random_device rd;
  std::mt19937 mt{rd()};
};

class RandomInt : public Random {
 public:
  RandomInt() = default;

  // Return a random integer in the range [min, max).
  // Use uniform distribution by default.
  int operator()(int min, int max);
};

int RandomInt::operator()(int min, int max) {
  std::uniform_int_distribution<> dist(min, max - 1);
  return dist(mt);
}

class RandomDouble : public Random {
 public:
  RandomDouble() = default;

  // Return a random double in the range [min, max).
  // Use uniform distribution by default.
  double operator()(double min, double max);
};

double RandomDouble::operator()(double min, double max) {
  std::uniform_real_distribution<> dist(min, max);
  return dist(mt);
}

}  // namespace spatialkv

#endif  // SPATIALKV_UTIL_RANDOM_H_
