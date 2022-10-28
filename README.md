# Spatial LevelDB

LevelDB with Spatial Data Indexing and Multi-Version Support.

## Build

### Prerequisites

- CMake
- C/C++ Compiler, e.g., GCC

### Clone the Repository

```bash
git clone --recurse-submodules git@github.com:hfziu/spatialkv.git spatialkv
```

### Build with CMake

```bash
cd spatialkv
cmake -H. -Bbuild

cd build
cmake --build .
```

### Run Tests

```bash
# inside `build` directory
ctest --verbose
```
