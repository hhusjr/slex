# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/zeroisone/projects/slang/slex

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/zeroisone/projects/slang/slex/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/istream.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/istream.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/istream.dir/flags.make

CMakeFiles/istream.dir/istream.c.o: CMakeFiles/istream.dir/flags.make
CMakeFiles/istream.dir/istream.c.o: ../istream.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/istream.dir/istream.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/istream.dir/istream.c.o   -c /Users/zeroisone/projects/slang/slex/istream.c

CMakeFiles/istream.dir/istream.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/istream.dir/istream.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/zeroisone/projects/slang/slex/istream.c > CMakeFiles/istream.dir/istream.c.i

CMakeFiles/istream.dir/istream.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/istream.dir/istream.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/zeroisone/projects/slang/slex/istream.c -o CMakeFiles/istream.dir/istream.c.s

# Object files for target istream
istream_OBJECTS = \
"CMakeFiles/istream.dir/istream.c.o"

# External object files for target istream
istream_EXTERNAL_OBJECTS =

libistream.a: CMakeFiles/istream.dir/istream.c.o
libistream.a: CMakeFiles/istream.dir/build.make
libistream.a: CMakeFiles/istream.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libistream.a"
	$(CMAKE_COMMAND) -P CMakeFiles/istream.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/istream.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/istream.dir/build: libistream.a

.PHONY : CMakeFiles/istream.dir/build

CMakeFiles/istream.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/istream.dir/cmake_clean.cmake
.PHONY : CMakeFiles/istream.dir/clean

CMakeFiles/istream.dir/depend:
	cd /Users/zeroisone/projects/slang/slex/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles/istream.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/istream.dir/depend
