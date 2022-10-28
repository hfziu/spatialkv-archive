cmake_minimum_required(VERSION 3.23)
project(smvkv)

set(CMAKE_CXX_STANDARD 17)

add_library(smvkv)

set(DB_CONNECTOR_PUBLIC_INCLUDE_DIR "include/db_connector")

target_include_directories(smvkv
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(smvkv
  PRIVATE
    "db_connector/leveldb_connector.cc"
  PUBLIC
    "${DB_CONNECTOR_PUBLIC_INCLUDE_DIR}/connector.h"
  )

include(FetchContent)
FetchContent_Declare(
  leveldb
  GIT_REPOSITORY https://github.com/google/leveldb.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(leveldb)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
include(GoogleTest)
function(smvkv_test filename)
  get_filename_component(test_name ${filename} NAME_WE)
  add_executable(${test_name} ${filename})
  target_link_libraries(${test_name} GTest::gtest_main smvkv leveldb)
  gtest_discover_tests(${test_name})
endfunction()

smvkv_test("db_connector/leveldb_connector_test.cc")