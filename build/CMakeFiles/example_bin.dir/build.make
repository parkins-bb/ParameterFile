# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build

# Include any dependencies generated for this target.
include CMakeFiles/example_bin.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/example_bin.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/example_bin.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/example_bin.dir/flags.make

CMakeFiles/example_bin.dir/example/example.cpp.o: CMakeFiles/example_bin.dir/flags.make
CMakeFiles/example_bin.dir/example/example.cpp.o: /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/example/example.cpp
CMakeFiles/example_bin.dir/example/example.cpp.o: CMakeFiles/example_bin.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/example_bin.dir/example/example.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/example_bin.dir/example/example.cpp.o -MF CMakeFiles/example_bin.dir/example/example.cpp.o.d -o CMakeFiles/example_bin.dir/example/example.cpp.o -c /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/example/example.cpp

CMakeFiles/example_bin.dir/example/example.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/example_bin.dir/example/example.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/example/example.cpp > CMakeFiles/example_bin.dir/example/example.cpp.i

CMakeFiles/example_bin.dir/example/example.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/example_bin.dir/example/example.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/example/example.cpp -o CMakeFiles/example_bin.dir/example/example.cpp.s

# Object files for target example_bin
example_bin_OBJECTS = \
"CMakeFiles/example_bin.dir/example/example.cpp.o"

# External object files for target example_bin
example_bin_EXTERNAL_OBJECTS =

/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: CMakeFiles/example_bin.dir/example/example.cpp.o
/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: CMakeFiles/example_bin.dir/build.make
/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: libparameter_lib.a
/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/lib/libyaml-cpp.a
/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin: CMakeFiles/example_bin.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_bin.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/example_bin.dir/build: /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/bin/example_bin
.PHONY : CMakeFiles/example_bin.dir/build

CMakeFiles/example_bin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/example_bin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/example_bin.dir/clean

CMakeFiles/example_bin.dir/depend:
	cd /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build /home/zhaixiuxi/workspace/HSF_docker_design/my_docker_tst/build/CMakeFiles/example_bin.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/example_bin.dir/depend
