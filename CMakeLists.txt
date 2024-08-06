cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 24.08)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenMP support
find_package(OpenMP REQUIRED)

# Set compiler flags
add_compile_options(-Wall -Wno-unknown-pragmas -fdiagnostics-color=always -g -O3 -mavx2 -mfma ${OpenMP_CXX_FLAGS})

# Set include directories
include_directories(include external/StreamCraft/src)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/StreamCraft/src/*.cpp")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags
target_link_libraries(${PROJECT_NAME} z ${OpenMP_CXX_LIBRARIES})
