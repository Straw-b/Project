# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/bin/cmake3

# The command to remove a file.
RM = /usr/bin/cmake3 -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wudu/work/linux-84/net-gobang/rest_rpc/game

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid

# Include any dependencies generated for this target.
include CMakeFiles/basic_server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/basic_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/basic_server.dir/flags.make

CMakeFiles/basic_server.dir/server/main.cpp.o: CMakeFiles/basic_server.dir/flags.make
CMakeFiles/basic_server.dir/server/main.cpp.o: ../../server/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/basic_server.dir/server/main.cpp.o"
	/opt/rh/devtoolset-7/root/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/basic_server.dir/server/main.cpp.o -c /home/wudu/work/linux-84/net-gobang/rest_rpc/game/server/main.cpp

CMakeFiles/basic_server.dir/server/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/basic_server.dir/server/main.cpp.i"
	/opt/rh/devtoolset-7/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wudu/work/linux-84/net-gobang/rest_rpc/game/server/main.cpp > CMakeFiles/basic_server.dir/server/main.cpp.i

CMakeFiles/basic_server.dir/server/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/basic_server.dir/server/main.cpp.s"
	/opt/rh/devtoolset-7/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wudu/work/linux-84/net-gobang/rest_rpc/game/server/main.cpp -o CMakeFiles/basic_server.dir/server/main.cpp.s

# Object files for target basic_server
basic_server_OBJECTS = \
"CMakeFiles/basic_server.dir/server/main.cpp.o"

# External object files for target basic_server
basic_server_EXTERNAL_OBJECTS =

basic_server: CMakeFiles/basic_server.dir/server/main.cpp.o
basic_server: CMakeFiles/basic_server.dir/build.make
basic_server: /usr/lib/libboost_system.so.1.75.0
basic_server: /usr/lib/libboost_filesystem.so.1.75.0
basic_server: CMakeFiles/basic_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable basic_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/basic_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/basic_server.dir/build: basic_server

.PHONY : CMakeFiles/basic_server.dir/build

CMakeFiles/basic_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/basic_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/basic_server.dir/clean

CMakeFiles/basic_server.dir/depend:
	cd /home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wudu/work/linux-84/net-gobang/rest_rpc/game /home/wudu/work/linux-84/net-gobang/rest_rpc/game /home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid /home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid /home/wudu/work/linux-84/net-gobang/rest_rpc/game/client/bulid/CMakeFiles/basic_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/basic_server.dir/depend
