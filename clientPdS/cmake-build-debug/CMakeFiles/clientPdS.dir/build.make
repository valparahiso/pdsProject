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
CMAKE_COMMAND = /home/simo/Scaricati/clion-2020.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/simo/Scaricati/clion-2020.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/simo/pdsProject/clientPdS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/simo/pdsProject/clientPdS/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/clientPdS.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/clientPdS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/clientPdS.dir/flags.make

CMakeFiles/clientPdS.dir/main.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/clientPdS.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/main.cpp.o -c /home/simo/pdsProject/clientPdS/main.cpp

CMakeFiles/clientPdS.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/main.cpp > CMakeFiles/clientPdS.dir/main.cpp.i

CMakeFiles/clientPdS.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/main.cpp -o CMakeFiles/clientPdS.dir/main.cpp.s

CMakeFiles/clientPdS.dir/tcp_client.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/tcp_client.cpp.o: ../tcp_client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/clientPdS.dir/tcp_client.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/tcp_client.cpp.o -c /home/simo/pdsProject/clientPdS/tcp_client.cpp

CMakeFiles/clientPdS.dir/tcp_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/tcp_client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/tcp_client.cpp > CMakeFiles/clientPdS.dir/tcp_client.cpp.i

CMakeFiles/clientPdS.dir/tcp_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/tcp_client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/tcp_client.cpp -o CMakeFiles/clientPdS.dir/tcp_client.cpp.s

CMakeFiles/clientPdS.dir/user.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/user.cpp.o: ../user.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/clientPdS.dir/user.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/user.cpp.o -c /home/simo/pdsProject/clientPdS/user.cpp

CMakeFiles/clientPdS.dir/user.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/user.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/user.cpp > CMakeFiles/clientPdS.dir/user.cpp.i

CMakeFiles/clientPdS.dir/user.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/user.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/user.cpp -o CMakeFiles/clientPdS.dir/user.cpp.s

CMakeFiles/clientPdS.dir/parameters_exception.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/parameters_exception.cpp.o: ../parameters_exception.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/clientPdS.dir/parameters_exception.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/parameters_exception.cpp.o -c /home/simo/pdsProject/clientPdS/parameters_exception.cpp

CMakeFiles/clientPdS.dir/parameters_exception.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/parameters_exception.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/parameters_exception.cpp > CMakeFiles/clientPdS.dir/parameters_exception.cpp.i

CMakeFiles/clientPdS.dir/parameters_exception.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/parameters_exception.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/parameters_exception.cpp -o CMakeFiles/clientPdS.dir/parameters_exception.cpp.s

CMakeFiles/clientPdS.dir/directory_exception.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/directory_exception.cpp.o: ../directory_exception.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/clientPdS.dir/directory_exception.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/directory_exception.cpp.o -c /home/simo/pdsProject/clientPdS/directory_exception.cpp

CMakeFiles/clientPdS.dir/directory_exception.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/directory_exception.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/directory_exception.cpp > CMakeFiles/clientPdS.dir/directory_exception.cpp.i

CMakeFiles/clientPdS.dir/directory_exception.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/directory_exception.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/directory_exception.cpp -o CMakeFiles/clientPdS.dir/directory_exception.cpp.s

CMakeFiles/clientPdS.dir/file_watcher.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/file_watcher.cpp.o: ../file_watcher.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/clientPdS.dir/file_watcher.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/file_watcher.cpp.o -c /home/simo/pdsProject/clientPdS/file_watcher.cpp

CMakeFiles/clientPdS.dir/file_watcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/file_watcher.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/file_watcher.cpp > CMakeFiles/clientPdS.dir/file_watcher.cpp.i

CMakeFiles/clientPdS.dir/file_watcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/file_watcher.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/file_watcher.cpp -o CMakeFiles/clientPdS.dir/file_watcher.cpp.s

CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o: ../filesystem_utility.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o -c /home/simo/pdsProject/clientPdS/filesystem_utility.cpp

CMakeFiles/clientPdS.dir/filesystem_utility.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/filesystem_utility.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/filesystem_utility.cpp > CMakeFiles/clientPdS.dir/filesystem_utility.cpp.i

CMakeFiles/clientPdS.dir/filesystem_utility.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/filesystem_utility.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/filesystem_utility.cpp -o CMakeFiles/clientPdS.dir/filesystem_utility.cpp.s

CMakeFiles/clientPdS.dir/hash_utility.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/hash_utility.cpp.o: ../hash_utility.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/clientPdS.dir/hash_utility.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/hash_utility.cpp.o -c /home/simo/pdsProject/clientPdS/hash_utility.cpp

CMakeFiles/clientPdS.dir/hash_utility.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/hash_utility.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/hash_utility.cpp > CMakeFiles/clientPdS.dir/hash_utility.cpp.i

CMakeFiles/clientPdS.dir/hash_utility.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/hash_utility.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/hash_utility.cpp -o CMakeFiles/clientPdS.dir/hash_utility.cpp.s

CMakeFiles/clientPdS.dir/JSON_utility.cpp.o: CMakeFiles/clientPdS.dir/flags.make
CMakeFiles/clientPdS.dir/JSON_utility.cpp.o: ../JSON_utility.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/clientPdS.dir/JSON_utility.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/clientPdS.dir/JSON_utility.cpp.o -c /home/simo/pdsProject/clientPdS/JSON_utility.cpp

CMakeFiles/clientPdS.dir/JSON_utility.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clientPdS.dir/JSON_utility.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simo/pdsProject/clientPdS/JSON_utility.cpp > CMakeFiles/clientPdS.dir/JSON_utility.cpp.i

CMakeFiles/clientPdS.dir/JSON_utility.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clientPdS.dir/JSON_utility.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simo/pdsProject/clientPdS/JSON_utility.cpp -o CMakeFiles/clientPdS.dir/JSON_utility.cpp.s

# Object files for target clientPdS
clientPdS_OBJECTS = \
"CMakeFiles/clientPdS.dir/main.cpp.o" \
"CMakeFiles/clientPdS.dir/tcp_client.cpp.o" \
"CMakeFiles/clientPdS.dir/user.cpp.o" \
"CMakeFiles/clientPdS.dir/parameters_exception.cpp.o" \
"CMakeFiles/clientPdS.dir/directory_exception.cpp.o" \
"CMakeFiles/clientPdS.dir/file_watcher.cpp.o" \
"CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o" \
"CMakeFiles/clientPdS.dir/hash_utility.cpp.o" \
"CMakeFiles/clientPdS.dir/JSON_utility.cpp.o"

# External object files for target clientPdS
clientPdS_EXTERNAL_OBJECTS =

clientPdS: CMakeFiles/clientPdS.dir/main.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/tcp_client.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/user.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/parameters_exception.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/directory_exception.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/file_watcher.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/filesystem_utility.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/hash_utility.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/JSON_utility.cpp.o
clientPdS: CMakeFiles/clientPdS.dir/build.make
clientPdS: CMakeFiles/clientPdS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable clientPdS"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clientPdS.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/clientPdS.dir/build: clientPdS

.PHONY : CMakeFiles/clientPdS.dir/build

CMakeFiles/clientPdS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/clientPdS.dir/cmake_clean.cmake
.PHONY : CMakeFiles/clientPdS.dir/clean

CMakeFiles/clientPdS.dir/depend:
	cd /home/simo/pdsProject/clientPdS/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/simo/pdsProject/clientPdS /home/simo/pdsProject/clientPdS /home/simo/pdsProject/clientPdS/cmake-build-debug /home/simo/pdsProject/clientPdS/cmake-build-debug /home/simo/pdsProject/clientPdS/cmake-build-debug/CMakeFiles/clientPdS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/clientPdS.dir/depend

