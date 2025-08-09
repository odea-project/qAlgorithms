# CMake settings for qAlgorithms
cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2 LANGUAGES CXX)

# Set C++ standard (do not also pass -std in add_compile_options)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: choose compiler externally via CMake or environment variable
# set(CMAKE_CXX_COMPILER "g++")

# Option to enable AddressSanitizer in Debug builds
option(V_ASAN "Enable AddressSanitizer" OFF)

# Common compiler flags for all configurations
add_compile_options(
  -Wall -Wpedantic -Wuninitialized -Wno-unknown-pragmas -Wformat -Wformat=2 -Wimplicit-fallthrough
  -mavx2 -march=native
  -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS
  -fno-math-errno
  ${OpenMP_CXX_FLAGS}
)

# Configuration-specific flags
# Debug: disable optimizations, enable debug symbols
# Release: enable optimizations
add_compile_options(
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-ggdb3>
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-g>
)

# In Release builds, define NDEBUG
# -> assert() calls and #ifndef NDEBUG debug code are removed
add_compile_definitions(
  $<$<CONFIG:Release>:NDEBUG>
)

# Optionally enable AddressSanitizer in Debug builds
if (V_ASAN)
  add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
  add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
endif()

# Include directories for headers
include_directories(include external/StreamCraft/src)

# Collect source files
file(GLOB_RECURSE SOURCES
  "src/*.cpp"
  "external/StreamCraft/src/*.cpp"
  "external/CDFlib/cdflib.cpp"
)

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags (link against zlib)
target_link_libraries(${PROJECT_NAME} PUBLIC z)
