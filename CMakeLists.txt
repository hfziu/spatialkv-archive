cmake_minimum_required(VERSION 3.17)
project(smvkv)

set(CMAKE_CXX_STANDARD 17)

add_library(smvkv)

# Include LevelDB
# TODO: sanitize the linker path
#       to detect libleveldb automatically or import leveldb as a sub-module
link_directories(${CMAKE_PREFIX_PATH}/lib /usr/local/lib)
include_directories(${CMAKE_PREFIX_PATH}/include /usr/local/include)

set(DB_CONNECTOR_PUBLIC_INCLUDE_DIR "include/db_connector")

target_include_directories(smvkv
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

target_sources(smvkv
  PRIVATE
  "db/format.h"
  "db_connector/leveldb_connector.cc"
  "util/encoder.cc"
  "util/encoder.h"
  "util/s2cell_id.cc"
  "util/s2cell_id.h"
  PUBLIC
  "${DB_CONNECTOR_PUBLIC_INCLUDE_DIR}/connector.h"
  )

# This project uses GoogleTest
# https://github.com/google/googletest
find_package(GTest REQUIRED)
if (GTest_FOUND)
# Build tests
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  enable_testing()
  include(GoogleTest)
  function(smvkv_test filename)
    get_filename_component(test_name ${filename} NAME_WE)
    add_executable(${test_name} ${filename})
    target_link_libraries(${test_name} GTest::gtest_main smvkv)
    target_link_libraries(${test_name} leveldb)
    gtest_discover_tests(${test_name})
  endfunction()

  smvkv_test("db_connector/leveldb_connector_test.cc")
  smvkv_test("util/s2_test.cc")
endif ()