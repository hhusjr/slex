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
include CMakeFiles/regexp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/regexp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/regexp.dir/flags.make

CMakeFiles/regexp.dir/regexp.c.o: CMakeFiles/regexp.dir/flags.make
CMakeFiles/regexp.dir/regexp.c.o: ../regexp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/regexp.dir/regexp.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/regexp.dir/regexp.c.o   -c /Users/zeroisone/projects/slang/slex/regexp.c

CMakeFiles/regexp.dir/regexp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/regexp.dir/regexp.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/zeroisone/projects/slang/slex/regexp.c > CMakeFiles/regexp.dir/regexp.c.i

CMakeFiles/regexp.dir/regexp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/regexp.dir/regexp.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/zeroisone/projects/slang/slex/regexp.c -o CMakeFiles/regexp.dir/regexp.c.s

# Object files for target regexp
regexp_OBJECTS = \
"CMakeFiles/regexp.dir/regexp.c.o"

# External object files for target regexp
regexp_EXTERNAL_OBJECTS =

libregexp.a: CMakeFiles/regexp.dir/regexp.c.o
libregexp.a: CMakeFiles/regexp.dir/build.make
libregexp.a: CMakeFiles/regexp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libregexp.a"
	$(CMAKE_COMMAND) -P CMakeFiles/regexp.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/regexp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/regexp.dir/build: libregexp.a

.PHONY : CMakeFiles/regexp.dir/build

CMakeFiles/regexp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/regexp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/regexp.dir/clean

CMakeFiles/regexp.dir/depend:
	cd /Users/zeroisone/projects/slang/slex/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug /Users/zeroisone/projects/slang/slex/cmake-build-debug/CMakeFiles/regexp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/regexp.dir/depend
