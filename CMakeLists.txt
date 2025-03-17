# cmake settings for qAlgorithms

### start build with ccmake . in /build to use the config options! ###

cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# required libraries
find_package(OpenMP REQUIRED)
message(STATUS "OpenMP_CXX_LIBRARIES: ${OpenMP_CXX_LIBRARIES}")
# find_package(ZLIB REQUIRED)
# message(STATUS "zlib found: ${ZLIB_INCLUDE_DIRS}, ${ZLIB_LIBRARIES}")


### use options by executing cmake with -(argname)=ON as a flag ###
# user option for enabling address sanitizer when on linux
option(V_ASAN "Enable AddressSanitizer" OFF)
# switch between compiling O0 and O2 without editing this file
# option(V_DEBUG "Compile with -O0" OFF)
# static compile the executeable
option(V_STATIC "Compile a statically linked executeable" OFF)

if(V_ASAN AND UNIX AND NOT APPLE)
  set(CMAKE_BUILD_TYPE "Debug-Linux")
else()
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# if(V_DEBUG)
#   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O0)
# else()
#   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O2)
# endif()

# Set compiler flags
add_compile_options(-Wall -Wuninitialized -Wno-unknown-pragmas -Wformat -Wformat=2 -Wimplicit-fallthrough
                    -fopenmp -mavx2 -march=native -O0 # -flto
                    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS # hardening flags
                    -fno-math-errno -g -ggdb3 ${OpenMP_CXX_FLAGS}) 

if(CMAKE_BUILD_TYPE MATCHES "Debug-Linux")
  add_compile_options(-fsanitize=address)
endif()

if(V_STATIC)
    message(STATUS "Enabling static compilation")
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -static)
endif()

# Set include directories
include_directories(include external/StreamCraft/src)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "external/StreamCraft/src/*.cpp")

# flags for static compile 
# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Linker flags
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenMP_CXX_LIBRARIES} z) # "z" is the linker flag for zlib, which is included as a header file
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug-Linux")
  set(
    CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS} -Werror -fsanitize=undefined -fsanitize=address
  )
  target_link_options(qAlgorithms
    BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address
  )
endif()
