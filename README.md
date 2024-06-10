# Spatial LevelDB

## Build

### Prerequisites

- CMake
- C/C++ Compiler, e.g., GCC

### Build with CMake

```bash
cd spatial-leveldb
cmake -H. -Bbuild

cd build
cmake --build .
```

### Run Tests

```bash
# inside `build` directory
ctest --verbose
```
