cmake_minimum_required(VERSION 3.13)
project(spatial_leveldb LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LEVELDB_BUILD_BENCHMARKS "Build LevelDB's benchmarks" ON)

if (WIN32)
  set(LEVELDB_PLATFORM_NAME LEVELDB_PLATFORM_WINDOWS)
  # TODO(cmumford): Make UNICODE configurable for Windows.
  add_definitions(-D_UNICODE -DUNICODE)
else (WIN32)
  set(LEVELDB_PLATFORM_NAME LEVELDB_PLATFORM_POSIX)
endif (WIN32)

include(CheckIncludeFile)
check_include_file("unistd.h" HAVE_UNISTD_H)

include(CheckLibraryExists)
check_library_exists(crc32c crc32c_value "" HAVE_CRC32C)
check_library_exists(snappy snappy_compress "" HAVE_SNAPPY)
check_library_exists(tcmalloc malloc "" HAVE_TCMALLOC)

include(CheckCXXSymbolExists)
# Using check_cxx_symbol_exists() instead of check_c_symbol_exists() because
# we're including the header from C++, and feature detection should use the same
# compiler language that the project will use later. Principles aside, some
# versions of do not expose fdatasync() in <unistd.h> in standard C mode
# (-std=c11), but do expose the function in standard C++ mode (-std=c++11).
check_cxx_symbol_exists(fdatasync "unistd.h" HAVE_FDATASYNC)
check_cxx_symbol_exists(F_FULLFSYNC "fcntl.h" HAVE_FULLFSYNC)
check_cxx_symbol_exists(O_CLOEXEC "fcntl.h" HAVE_O_CLOEXEC)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Disable C++ exceptions.
  string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c-")
  add_definitions(-D_HAS_EXCEPTIONS=0)

  # Disable RTTI.
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Enable strict prototype warnings for C code in clang and gcc.
  if(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
  endif(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")

  # Disable C++ exceptions.
  string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Disable RTTI.
  string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

# Test whether -Wthread-safety is available. See
# https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wthread-safety HAVE_CLANG_THREAD_SAFETY)

# Used by googletest.
check_cxx_compiler_flag(-Wno-missing-field-initializers
  LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)

include(CheckCXXSourceCompiles)

# Test whether C++17 __has_include is available.
check_cxx_source_compiles("
#if defined(__has_include) &&  __has_include(<string>)
#include <string>
#endif
int main() { std::string str; return 0; }
" HAVE_CXX17_HAS_INCLUDE)

set(LEVELDB_PUBLIC_INCLUDE_DIR "include/leveldb")
set(LEVELDB_SPATIAL_PUBLIC_INCLUDE_DIR "include/spatial")
set(LEVELDB_PORT_CONFIG_DIR "include/port")

configure_file(
  "port/port_config.h.in"
  "${PROJECT_BINARY_DIR}/${LEVELDB_PORT_CONFIG_DIR}/port_config.h"
)

include_directories(
  "${PROJECT_BINARY_DIR}/include"
  "."
)

# Must be included before CMAKE_INSTALL_INCLUDEDIR is used.
include(GNUInstallDirs)

add_library(spatial_leveldb "")
target_sources(spatial_leveldb
  PRIVATE
  "db/builder.cc"
  "db/builder.h"
  "db/c.cc"
  "db/db_impl.cc"
  "db/db_impl.h"
  "db/db_iter.h"
  "db/db_iter.cc"
  "db/dbformat.cc"
  "db/dbformat.h"
  "db/dumpfile.cc"
  "db/filename.cc"
  "db/filename.h"
  "db/log_format.h"
  "db/log_reader.cc"
  "db/log_reader.h"
  "db/log_writer.cc"
  "db/log_writer.h"
  "db/memtable.cc"
  "db/memtable.h"
  "db/repair.cc"
  "db/skiplist.h"
  "db/snapshot.h"
  "db/table_cache.cc"
  "db/table_cache.h"
  "db/version_edit.cc"
  "db/version_edit.h"
  "db/version_set.cc"
  "db/version_set.h"
  "db/write_batch.cc"
  "db/write_batch_internal.h"
  "port/port.h"
  "port/port_stdcxx.h"
  "port/thread_annotations.h"
  "spatial/curve.h"
  "spatial/curve.cc"
  "spatial/index.h"
  "table/block_builder.cc"
  "table/block_builder.h"
  "table/block.cc"
  "table/block.h"
  "table/filter_block.cc"
  "table/filter_block.h"
  "table/format.cc"
  "table/format.h"
  "table/iterator_wrapper.h"
  "table/iterator.cc"
  "table/merger.cc"
  "table/merger.h"
  "table/table_builder.cc"
  "table/table.cc"
  "table/two_level_iterator.cc"
  "table/two_level_iterator.h"
  "util/arena.cc"
  "util/arena.h"
  "util/bloom.cc"
  "util/cache.cc"
  "util/coding.cc"
  "util/coding.h"
  "util/comparator.cc"
  "util/crc32c.cc"
  "util/crc32c.h"
  "util/env.cc"
  "util/filter_policy.cc"
  "util/hash.cc"
  "util/hash.h"
  "util/logging.cc"
  "util/logging.h"
  "util/mutexlock.h"
  "util/no_destructor.h"
  "util/options.cc"
  "util/random.h"
  "util/status.cc"

  # Only CMake 3.3+ supports PUBLIC sources in targets exported by "install".
  $<$<VERSION_GREATER:CMAKE_VERSION,3.2>:PUBLIC>
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/c.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/cache.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/comparator.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/db.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/dumpfile.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/env.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/export.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/filter_policy.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/format.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/iterator.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/options.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/slice.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/status.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/table.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/table_builder.h"
  "${LEVELDB_PUBLIC_INCLUDE_DIR}/write_batch.h"
  "${LEVELDB_SPATIAL_PUBLIC_INCLUDE_DIR}/format.h"
  )

if (WIN32)
  target_sources(spatial_leveldb
    PRIVATE
    "util/env_windows.cc"
    "util/windows_logger.h"
    )
else (WIN32)
  target_sources(spatial_leveldb
    PRIVATE
    "util/env_posix.cc"
    "util/posix_logger.h"
    )
endif (WIN32)

# MemEnv is not part of the interface and could be pulled to a separate library.
target_sources(spatial_leveldb
  PRIVATE
  "helpers/memenv/memenv.cc"
  "helpers/memenv/memenv.h"
  )

target_include_directories(spatial_leveldb
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

set_target_properties(spatial_leveldb
  PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

target_compile_definitions(spatial_leveldb
  PRIVATE
  # Used by include/export.h when building shared libraries.
  LEVELDB_COMPILE_LIBRARY
  # Used by port/port.h.
  ${LEVELDB_PLATFORM_NAME}=1
  )

if (NOT HAVE_CXX17_HAS_INCLUDE)
  target_compile_definitions(spatial_leveldb
    PRIVATE
    LEVELDB_HAS_PORT_CONFIG_H=1
    )
endif (NOT HAVE_CXX17_HAS_INCLUDE)

if(HAVE_CLANG_THREAD_SAFETY)
  target_compile_options(spatial_leveldb
    PUBLIC
      -Werror -Wthread-safety)
endif(HAVE_CLANG_THREAD_SAFETY)

# =======
# Testing
# =======
enable_testing()

# Prevent overriding the parent project's compiler/linker settings on Windows.
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(install_gtest OFF)
  set(install_gmock OFF)
  set(build_gmock ON)

# This project is tested using GoogleTest.
add_subdirectory("third_party/googletest")

# GoogleTest triggers a missing field initializers warning.
  if(LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
  endif(LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)

function(spatial_leveldb_test test_file)
  get_filename_component(test_target_name "${test_file}" NAME_WE)
  add_executable("${test_target_name}" "")
  target_sources("${test_target_name}"
    PRIVATE
    "${PROJECT_BINARY_DIR}/${LEVELDB_PORT_CONFIG_DIR}/port_config.h"
    "util/testutil.cc"
    "util/testutil.h"
    "${test_file}"
    )
  target_link_libraries("${test_target_name}" spatial_leveldb gmock gtest)
  target_compile_definitions("${test_target_name}"
    PRIVATE
    ${LEVELDB_PLATFORM_NAME}=1
    )
  if (NOT HAVE_CXX17_HAS_INCLUDE)
    target_compile_definitions("${test_target_name}"
      PRIVATE
      LEVELDB_HAS_PORT_CONFIG_H=1
      )
  endif (NOT HAVE_CXX17_HAS_INCLUDE)

  add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(spatial_leveldb_test)

spatial_leveldb_test("db/dbformat_test.cc")
spatial_leveldb_test("db/memtable_test.cc")
spatial_leveldb_test("db/skiplist_test.cc")
# spatial_leveldb_test("db/version_edit_test.cc")
# TODO: Fix WriteBatch for multi-version
spatial_leveldb_test("db/write_batch_test.cc")

#spatial_leveldb_test("helpers/memenv/memenv_test.cc")

spatial_leveldb_test("spatial/curve_test.cc")

spatial_leveldb_test("util/arena_test.cc")
spatial_leveldb_test("util/coding_test.cc")
spatial_leveldb_test("util/crc32c_test.cc")
spatial_leveldb_test("util/hash_test.cc")
spatial_leveldb_test("util/logging_test.cc")
spatial_leveldb_test("util/no_destructor_test.cc")

# TODO(costan): This test also uses
#               "util/env_{posix|windows}_test_helper.h"
if (WIN32)
  spatial_leveldb_test("util/env_windows_test.cc")
else (WIN32)
  spatial_leveldb_test("util/env_posix_test.cc")
endif (WIN32)

# ==========
# Benchmarks
# ==========

if(LEVELDB_BUILD_BENCHMARKS)
  function(leveldb_benchmark bench_file)
    get_filename_component(bench_target_name "${bench_file}" NAME_WE)

    add_executable("${bench_target_name}" "")
    target_sources("${bench_target_name}"
      PRIVATE
      "${PROJECT_BINARY_DIR}/${LEVELDB_PORT_CONFIG_DIR}/port_config.h"
      "util/histogram.cc"
      "util/histogram.h"
      "util/testutil.cc"
      "util/testutil.h"

      "${bench_file}"
      )
    target_link_libraries("${bench_target_name}" spatial_leveldb gmock gtest)
    target_compile_definitions("${bench_target_name}"
      PRIVATE
      ${LEVELDB_PLATFORM_NAME}=1
      )
    if (NOT HAVE_CXX17_HAS_INCLUDE)
      target_compile_definitions("${bench_target_name}"
        PRIVATE
        LEVELDB_HAS_PORT_CONFIG_H=1
        )
    endif(NOT HAVE_CXX17_HAS_INCLUDE)
  endfunction(leveldb_benchmark)

  # if(NOT BUILD_SHARED_LIBS)
  #   leveldb_benchmark("benchmarks/db_spatial_bench.cc")
  # endif(NOT BUILD_SHARED_LIBS)

#  check_library_exists(sqlite3 sqlite3_open "" HAVE_SQLITE3)
#  if(HAVE_SQLITE3)
#    leveldb_benchmark("benchmarks/db_bench_sqlite3.cc")
#    target_link_libraries(db_bench_sqlite3 sqlite3)
#  endif(HAVE_SQLITE3)

  # check_library_exists is insufficient here because the library names have
  # different manglings when compiled with clang or gcc, at least when installed
  # with Homebrew on Mac.
  set(OLD_CMAKE_REQURED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
  list(APPEND CMAKE_REQUIRED_LIBRARIES kyotocabinet)
  check_cxx_source_compiles("
#include <kcpolydb.h>

int main() {
  kyotocabinet::TreeDB* db = new kyotocabinet::TreeDB();
  delete db;
  return 0;
}
  "  HAVE_KYOTOCABINET)
  set(CMAKE_REQUIRED_LIBRARIES ${OLD_CMAKE_REQURED_LIBRARIES})
#  if(HAVE_KYOTOCABINET)
#    leveldb_benchmark("benchmarks/db_bench_tree_db.cc")
#    target_link_libraries(db_bench_tree_db kyotocabinet)
#  endif(HAVE_KYOTOCABINET)
endif(LEVELDB_BUILD_BENCHMARKS)