# cmake settings for qAlgorithms

cmake_minimum_required(VERSION 3.12)
project(qAlgorithms VERSION 0.2 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 26)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++") # requires std::sqrt to be constexpr, either g++ or c++26 and later

## Set compiler flags
option(OPTIMISE "turn on optimisations" OFF)

if (false)
    add_compile_options(-Ofast) 
    # hardening flags 
    add_compile_options(-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3)
else()
    add_compile_options(-O0)
endif()

# debug-related flags
add_compile_options(-g -ggdb3)

add_compile_options(-std=c++2c -fno-math-errno -mavx2 -march=native 
                    -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS)

# enabled warnings
add_compile_options(-Wall -Wpedantic -Wuninitialized -Wno-unknown-pragmas -Wformat=2 -Wformat-truncation -Wimplicit-fallthrough 
                    -Wshadow -Wnon-virtual-dtor -Wextra -Wcast-align -Wunused -Wduplicated-cond -Wduplicated-branches
                    -Wlogical-op -Wnull-dereference -Wuseless-cast -Wchanges-meaning) 

# flags for static compile 
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# warnings that produce too much noise when building - @todo add these in eventually
# add_compile_options(-Wdouble-promotion -Wconversion -Weffc++)
# add_compile_options(-Wconversion)

# add_compile_options(-fsanitize=address) # @todo find a way to use this (not supported under mingw)
# add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

# lets make the directories into variables so we dont have to type CMAKE_CURRENT_SOURCE_DIR everywhere... 
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")        # Directory containing test files and shared test resources
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external") # Directory containing external headers/libs
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")        # Directory containing the Source Files
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")   # Directory containing the Include Files

# Set include directories
include_directories(include "external/simdutf/" "external/cephes")  # @todo this is terrible practice. Lets not do it

# Source files
# TODO we currently list external files manually. Thats not great. 
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp" "${EXTERNAL_DIR}/CDFlib/cdflib.cpp" "${EXTERNAL_DIR}/simdutf/simdutf.cpp" "${EXTERNAL_DIR}/cephes/cephes.cpp")
list(FILTER SOURCES EXCLUDE REGEX "qalgorithms_main\\.cpp") # Remove test/main file(s) from the library list

# Now we make a library from our sources that we can then link main.cpp and all the tests against
add_library(qalgo_lib ${SOURCES})
# Expose include dirs to consumers (app + tests)
target_include_directories(qalgo_lib PUBLIC
    ${INCLUDE_DIR}
    ${TESTS_DIR}/shared
    ${EXTERNAL_DIR}
)

# link against zlib on windows (?) // does not work @todo
if(WIN32)
    set(ZLIB_INCLUDE_DIR "C:/msys64/ucrt64/include")
    set(ZLIB_LIBRARY "C:/msys64/ucrt64/lib/libz.a")
    set(ZLIB_USE_STATIC_LIBS "ON")
endif()
find_package(ZLIB REQUIRED)

# PROGRAM SOURCE
add_executable(${PROJECT_NAME} "${SOURCE_DIR}/qalgorithms_main.cpp")
target_link_libraries(${PROJECT_NAME} qalgo_lib ZLIB::ZLIB) 

# TESTS
include(CTest)
enable_testing()

file(GLOB_RECURSE TESTFILES "${TESTS_DIR}/*.cpp")
list(FILTER TESTFILES EXCLUDE REGEX ".*/test/shared/.*") # lets just assume that no one is stupid enough to have a second test/shared folder in this project

# Make the shared test code into a small library
file(GLOB_RECURSE TEST_SHARED_SOURCES "${TESTS_DIR}/shared/*.cpp")
add_library(test_shared_lib STATIC ${TEST_SHARED_SOURCES})
# TODO not sure which dirs we need to include here. 
target_include_directories(test_shared_lib PUBLIC ${TESTS_DIR}/shared ${INCLUDE_DIR} ${EXTERNAL_DIR})

# Now lets make the tests
foreach(src IN LISTS TESTFILES)
    # create a unique target name from relative path under tests/
    file(RELATIVE_PATH rel "${TESTS_DIR}" "${src}")
    string(REPLACE "/" "_" rel_safe "${rel}") # replace / with _
    string(REGEX REPLACE "\\.cpp$" "" rel_safe "${rel_safe}") # remove .cpp extension from filename
    set(target_name "test_${rel_safe}")
    message(NEW_TEST: "${target_name}")

    add_executable(${target_name} "${src}")
    set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests_bin)
    target_link_libraries(${target_name} PRIVATE qalgo_lib test_shared_lib PUBLIC  ZLIB::ZLIB)
    add_test(NAME ${target_name} COMMAND ${target_name})
endforeach()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)