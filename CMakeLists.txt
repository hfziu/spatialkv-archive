cmake_minimum_required(VERSION 3.17)
project(spatialkv)

set(CMAKE_CXX_STANDARD 20)

# LevelDB platform definitions (used by port/port.h)
if (WIN32)
  set(LEVELDB_PLATFORM_NAME LEVELDB_PLATFORM_WINDOWS)
  # TODO(cmumford): Make UNICODE configurable for Windows.
  add_definitions(-D_UNICODE -DUNICODE)
else (WIN32)
  set(LEVELDB_PLATFORM_NAME LEVELDB_PLATFORM_POSIX)
endif (WIN32)

add_library(spatialkv)

# Include LevelDB
# TODO: sanitize the linker path
#       to detect libleveldb automatically or import leveldb as a sub-module
link_directories(${CMAKE_PREFIX_PATH}/lib /usr/local/lib /opt/homebrew/lib)
include_directories(
  "${PROJECT_BINARY_DIR}/include"
  "."
  ${CMAKE_PREFIX_PATH}/include
  /usr/local/include
)

set(DB_CONNECTOR_PUBLIC_INCLUDE_DIR "include/db_connector")
set(SPATIALKV_PUBLIC_INCLUDE_DIR "include/spatialkv")

find_package(Protobuf CONFIG)
find_package(Protobuf REQUIRED)
set(protobuf_MODULE_COMPATIBLE true)
if (Protobuf_FOUND)
  message(STATUS "Using protobuf ${Protobuf_VERSION}")
  set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
  include_directories(${Protobuf_INCLUDE_DIRS})
  target_link_libraries(spatialkv ${Protobuf_LIBRARIES})
  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS "proto/values.proto")
  target_include_directories(spatialkv PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_sources(spatialkv
    PRIVATE
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )
endif ()

target_include_directories(spatialkv
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

target_sources(spatialkv
  PRIVATE
  "db_connector/leveldb_connector.cc"
  "db/db_impl.cc"
  "db/db_impl.h"
  "db/keyformat.cc"
  "port/port.h"
  "port/port_stdcxx.h"
  "port/thread_annotations.h"
  "util/coding.h"
  "util/encoder.cc"
  "util/encoder.h"
  "util/no_destructor.h"
  "util/random.h"
  "util/s2_util.h"
  PUBLIC
  "${DB_CONNECTOR_PUBLIC_INCLUDE_DIR}/connector.h"
  "${SPATIALKV_PUBLIC_INCLUDE_DIR}/db.h"
  "${SPATIALKV_PUBLIC_INCLUDE_DIR}/dbformat.h"
  "${SPATIALKV_PUBLIC_INCLUDE_DIR}/keyformat.h"
  )

# TODO: I have to explicitly link specific Abseil libraries for Protobuf to compile.
# https://github.com/protocolbuffers/protobuf/issues/12292
target_link_libraries(spatialkv leveldb s2 absl_log_internal_message absl_log_internal_check_op)
target_compile_definitions(spatialkv PRIVATE
  # LevelDB platform definitions (used by port/port.h)
  ${LEVELDB_PLATFORM_NAME}=1)

# This project uses GoogleTest
# https://github.com/google/googletest
find_package(GTest REQUIRED)
if (GTest_FOUND)
  # Build tests
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  enable_testing()
  include(GoogleTest)
  function(spatialkv_test filename)
    get_filename_component(test_name ${filename} NAME_WE)
    add_executable(${test_name} ${filename})
    target_link_libraries(${test_name} GTest::gtest_main spatialkv)
    gtest_discover_tests(${test_name})
  endfunction()

  spatialkv_test("db/db_test.cc")
  spatialkv_test("db/keyformat_test.cc")
  spatialkv_test("db_connector/leveldb_connector_test.cc")
  spatialkv_test("util/coding_test.cc")
  spatialkv_test("util/encoder_test.cc")
  spatialkv_test("util/no_destructor_test.cc")
  spatialkv_test("util/s2_test.cc")
endif ()
