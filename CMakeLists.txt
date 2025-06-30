# cmake settings for qAlgorithms

cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 26)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++") # clang does not work<

option(V_ASAN "Enable AddressSanitizer" OFF)

# Set compiler flags
add_compile_options(-Wall -Wpedantic -Wuninitialized -Wno-unknown-pragmas -Wformat -Wformat=2 -Wimplicit-fallthrough 
                    -mavx2 -march=native -O2 -std=c++2c # c++26 standard used
                    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS # hardening flags
                    -fno-math-errno -g -ggdb3 ${OpenMP_CXX_FLAGS}) 

# add_compile_options(-fsanitize=address)

# Set include directories
include_directories(include "external/simdutf/")

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/CDFlib/cdflib.cpp" "external/simdutf/simdutf.cpp")

# flags for static compile 
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags
target_link_libraries(${PROJECT_NAME} PUBLIC z) # "z" is the linker flag for zlib