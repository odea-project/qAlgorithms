cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

if(UNIX)
  set(CMAKE_BUILD_TYPE "Debug-Linux")
else()
  set(CMAKE_BUILD_TYPE "Debug")
endif()
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# OpenMP support
find_package(OpenMP REQUIRED)
# find_package(Matplot++ REQUIRED)

# Set compiler flags
add_compile_options(-Wall -Wuninitialized -Wno-unknown-pragmas -fdiagnostics-color=always 
                    -fopenmp -mavx2 -march=native 
                    -O2 -fno-math-errno -ggdb3 ${OpenMP_CXX_FLAGS}) #  -O3

if(CMAKE_BUILD_TYPE MATCHES "Debug-Linux")
  add_compile_options(-fsanitize=address)
endif()

# Set include directories
include_directories(include external/StreamCraft/src)
# add_subdirectory(external/matplotplusplus)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/StreamCraft/src/*.cpp")

# flags for static compile 
# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags
# target_link_libraries(${PROJECT_NAME} PUBLIC Matplot++::matplot z ${OpenMP_CXX_LIBRARIES})
target_link_libraries(${PROJECT_NAME} z ${OpenMP_CXX_LIBRARIES}) #

if(CMAKE_BUILD_TYPE MATCHES "Debug-Linux")
  set(
    CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -Werror -fsanitize=undefined -fsanitize=address"
  )
  target_link_options(qAlgorithms
    BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address
  )
endif()
