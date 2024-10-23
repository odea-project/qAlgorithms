cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenMP support
find_package(OpenMP REQUIRED)

# zlib support
find_package(ZLIB)

# Manually set zlib paths if not found
if (NOT ZLIB_FOUND)
    set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/zlib131")
    set(ZLIB_BUILD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/zlib131/build")
    set(ZLIB_LIBRARY "${CMAKE_SOURCE_DIR}/external/zlib131/build/libzlib.dll")
    set(ZLIB_LIBRARIES "${ZLIB_LIBRARY}")
endif()

# Set compiler flags
if (MSVC)
    add_compile_options(/W4 /openmp /D "_CRT_SECURE_NO_WARNINGS")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/RTC1 /Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra -O3 -fopenmp)
endif()

# Set include directories
include_directories(include external/StreamCraft/src ${ZLIB_INCLUDE_DIR} ${ZLIB_BUILD_INCLUDE_DIR})

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/StreamCraft/src/*.cpp")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags
target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX ${ZLIB_LIBRARIES})