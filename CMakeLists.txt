# cmake settings for qAlgorithms

cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 26)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++") # clang does not work<

option(V_ASAN "Enable AddressSanitizer" OFF)

## Set compiler flags

# compile options - optimisation / required
add_compile_options(-std=c++2c -fno-math-errno -mavx2 -march=native -O2)

# debug-related flags
add_compile_options(-g -ggdb3)

# hardening flags
add_compile_options(-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS)

# enabled warnings
add_compile_options(-Wall -Wpedantic -Wuninitialized -Wno-unknown-pragmas -Wformat=2 -Wformat-truncation -Wimplicit-fallthrough 
                    -Wshadow -Wnon-virtual-dtor -Wextra -Wcast-align -Wunused -Wduplicated-cond -Wduplicated-branches
                    -Wlogical-op -Wnull-dereference -Wuseless-cast -Wchanges-meaning) 

# warnings that produce too much noise when building - @todo add these in eventually
# add_compile_options(-Wdouble-promotion -Wconversion -Weffc++)
# add_compile_options(-Wconversion)

# add_compile_options(-fsanitize=address) # @todo find a way to use this (not supported under mingw)
# add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

# Set include directories
include_directories(include "external/simdutf/")

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/CDFlib/cdflib.cpp" "external/simdutf/simdutf.cpp")

# flags for static compile 
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} "tools/qalgorithms_main.cpp")

add_executable(tests ${SOURCES} "tools/tests.cpp")

# Linker flags
target_link_libraries(${PROJECT_NAME} PUBLIC z) # "z" is the linker flag for zlib
target_link_libraries(tests PUBLIC z) 