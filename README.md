# SpatialKV

## Dependencies

- [s2geometry](https://github.com/google/s2geometry)
- [leveldb](https://github.com/google/leveldb)
- [googletest](https://github.com/google/googletest)

## Build with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .

# Run tests
ctest --verbose
```

## Previous version

This is a branch under development. In [`main`](https://github.com/hfziu/spatialkv/tree/main) branch, the previous version is implemented by extending leveldb.
