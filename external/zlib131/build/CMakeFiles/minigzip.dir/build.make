# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build

# Include any dependencies generated for this target.
include CMakeFiles/minigzip.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/minigzip.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/minigzip.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/minigzip.dir/flags.make

CMakeFiles/minigzip.dir/test/minigzip.c.obj: CMakeFiles/minigzip.dir/flags.make
CMakeFiles/minigzip.dir/test/minigzip.c.obj: CMakeFiles/minigzip.dir/includes_C.rsp
CMakeFiles/minigzip.dir/test/minigzip.c.obj: C:/Users/grenn/Desktop/clean_op/qAlgorithms/external/zlib131/test/minigzip.c
CMakeFiles/minigzip.dir/test/minigzip.c.obj: CMakeFiles/minigzip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/minigzip.dir/test/minigzip.c.obj"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/minigzip.dir/test/minigzip.c.obj -MF CMakeFiles\minigzip.dir\test\minigzip.c.obj.d -o CMakeFiles\minigzip.dir\test\minigzip.c.obj -c C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\test\minigzip.c

CMakeFiles/minigzip.dir/test/minigzip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/minigzip.dir/test/minigzip.c.i"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\test\minigzip.c > CMakeFiles\minigzip.dir\test\minigzip.c.i

CMakeFiles/minigzip.dir/test/minigzip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/minigzip.dir/test/minigzip.c.s"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\test\minigzip.c -o CMakeFiles\minigzip.dir\test\minigzip.c.s

# Object files for target minigzip
minigzip_OBJECTS = \
"CMakeFiles/minigzip.dir/test/minigzip.c.obj"

# External object files for target minigzip
minigzip_EXTERNAL_OBJECTS =

minigzip.exe: CMakeFiles/minigzip.dir/test/minigzip.c.obj
minigzip.exe: CMakeFiles/minigzip.dir/build.make
minigzip.exe: libzlib.dll.a
minigzip.exe: CMakeFiles/minigzip.dir/linkLibs.rsp
minigzip.exe: CMakeFiles/minigzip.dir/objects1.rsp
minigzip.exe: CMakeFiles/minigzip.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable minigzip.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\minigzip.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/minigzip.dir/build: minigzip.exe
.PHONY : CMakeFiles/minigzip.dir/build

CMakeFiles/minigzip.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\minigzip.dir\cmake_clean.cmake
.PHONY : CMakeFiles/minigzip.dir/clean

CMakeFiles/minigzip.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131 C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131 C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build C:\Users\grenn\Desktop\clean_op\qAlgorithms\external\zlib131\build\CMakeFiles\minigzip.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/minigzip.dir/depend

