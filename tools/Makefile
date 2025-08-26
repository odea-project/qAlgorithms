# build and run tests

CC = gcc
CXX = g++

SRC_DIR = ../src
INC_DIR = ../include
OBJ_DIR = ../obj

SOURCES = $(wildcard $(SRC_DIR)/*.cpp) ../external/CDFlib/cdflib.cpp ../external/pugixml-1.14/src/pugixml.cpp -I../external/simdutf/simdutf.cpp
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# OBJECTS = tests.o \
# 		  qalgorithms_input_output.o qalgorithms_metafiles.o qalgorithms_qbin.o qalgorithms_qpattern.o \
# 		  qalgorithms_qpeaks.o qalgorithms_read_file.o qalgorithms_utils.o

# the commented out warnings produce too much noise
WARNINGS = -Wall -Wpedantic -Wuninitialized -Wno-unknown-pragmas -Wformat=2 -Wformat-truncation -Wimplicit-fallthrough \
		   -Wshadow -Wnon-virtual-dtor -Wextra -Wcast-align -Wunused -Wduplicated-cond -Wduplicated-branches \
           -Wlogical-op -Wnull-dereference -Wuseless-cast -Wchanges-meaning # -Wconversion # -Wdouble-promotion # -Weffc++

OPTIONS_DEBUG = -g -ggdb3

OPTIMISE = -std=c++2c -fno-math-errno -mavx2 -march=native -O2

FORTIFY = -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fdiagnostics-color=always -fstack-clash-protection -D_GLIBCXX_ASSERTIONS

INCLUDES = -I../include -I../external/CDFlib -I../external/pugixml-1.14/src -I../external/simdutf

CXXFLAGS = $(OPTIONS_DEBUG) $(OPTIMISE) $(FORTIFY) $(INCLUDES)

testrun: $(OBJECTS)
	$(CXX) -o testrun $(OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@mkdir -p $(OBJ_DIR)
# 	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# @todo: add choice of main function (move main outside of src? and add linking to libs)

.PHONY: clean
clean: 
	rm -f testrun &(OBJECTS)