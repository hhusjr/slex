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
include CMakeFiles/scanner.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/scanner.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/scanner.dir/flags.make

CMakeFiles/scanner.dir/scanner.c.o: CMakeFiles/scanner.dir/flags.make
CMakeFiles/scanner.dir/scanner.c.o: ../scanner.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/scanner.dir/scanner.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/scanner.dir/scanner.c.o   -c /Users/zeroisone/projects/slang/slex/scanner.c

CMakeFiles/scanner.dir/scanner.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/scanner.dir/scanner.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/zeroisone/projects/slang/slex/scanner.c > CMakeFiles/scanner.dir/scanner.c.i

CMakeFiles/scanner.dir/scanner.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/scanner.dir/scanner.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/zeroisone/projects/slang/slex/scanner.c -o CMakeFiles/scanner.dir/scanner.c.s

# Object files for target scanner
scanner_OBJECTS = \
"CMakeFiles/scanner.dir/scanner.c.o"

# External object files for target scanner
scanner_EXTERNAL_OBJECTS =

libscanner.a: CMakeFiles/scanner.dir/scanner.c.o
libscanner.a: CMakeFiles/scanner.dir/build.make
libscanner.a: CMakeFiles/scanner.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libscanner.a"
	$(CMAKE_COMMAND) -P CMakeFiles/scanner.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/scanner.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/scanner.dir/build: libscanner.a

.PHONY : CMakeFiles/scanner.dir/build

CMakeFiles/scanner.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/scanner.dir/cmake_clean.cmake
.PHONY : CMakeFiles/scanner.dir/clean

CMakeFiles/scanner.dir/depend:
	cd /Users/zeroisone/projects/slang/slex/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles/scanner.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/scanner.dir/depend

