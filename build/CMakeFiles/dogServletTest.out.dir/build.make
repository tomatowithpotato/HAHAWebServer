# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build

# Include any dependencies generated for this target.
include CMakeFiles/dogServletTest.out.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dogServletTest.out.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dogServletTest.out.dir/flags.make

CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o: CMakeFiles/dogServletTest.out.dir/flags.make
CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o: ../tests/test_servlet.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o -c /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/tests/test_servlet.cc

CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/tests/test_servlet.cc > CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.i

CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/tests/test_servlet.cc -o CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.s

# Object files for target dogServletTest.out
dogServletTest_out_OBJECTS = \
"CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o"

# External object files for target dogServletTest.out
dogServletTest_out_EXTERNAL_OBJECTS =

../bin/dogServletTest.out: CMakeFiles/dogServletTest.out.dir/tests/test_servlet.cc.o
../bin/dogServletTest.out: CMakeFiles/dogServletTest.out.dir/build.make
../bin/dogServletTest.out: ../lib/libhaha.so
../bin/dogServletTest.out: CMakeFiles/dogServletTest.out.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/dogServletTest.out"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dogServletTest.out.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dogServletTest.out.dir/build: ../bin/dogServletTest.out

.PHONY : CMakeFiles/dogServletTest.out.dir/build

CMakeFiles/dogServletTest.out.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dogServletTest.out.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dogServletTest.out.dir/clean

CMakeFiles/dogServletTest.out.dir/depend:
	cd /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build /home/lighthouse/MyCode/linux/linux_network/HAHAWebServer/build/CMakeFiles/dogServletTest.out.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dogServletTest.out.dir/depend

