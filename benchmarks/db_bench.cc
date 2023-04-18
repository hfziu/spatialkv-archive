//
// Created by xiaofzhao4 on 4/3/2023.
//

//#include <format>  // GCC libstdc++ 13+ or Clang libc++ 14+ required

namespace spatialkv {

class Benchmark {
 private:
  void PrintHeader() {

  }
 public:
  Benchmark() = default;

  void Run() {}
};

}  // namespace spatialkv

int main() {
  spatialkv::Benchmark benchmark;
  benchmark.Run();
  return 0;
}