# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build

# Include any dependencies generated for this target.
include components/my_lib_2/CMakeFiles/my_lib_2.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include components/my_lib_2/CMakeFiles/my_lib_2.dir/compiler_depend.make

# Include the progress variables for this target.
include components/my_lib_2/CMakeFiles/my_lib_2.dir/progress.make

# Include the compile flags for this target's objects.
include components/my_lib_2/CMakeFiles/my_lib_2.dir/flags.make

components/my_lib_2/CMakeFiles/my_lib_2.dir/codegen:
.PHONY : components/my_lib_2/CMakeFiles/my_lib_2.dir/codegen

components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o: components/my_lib_2/CMakeFiles/my_lib_2.dir/flags.make
components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o: /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/components/my_lib_2/my_lib_2.c
components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o: components/my_lib_2/CMakeFiles/my_lib_2.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o"
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o -MF CMakeFiles/my_lib_2.dir/my_lib_2.c.o.d -o CMakeFiles/my_lib_2.dir/my_lib_2.c.o -c /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/components/my_lib_2/my_lib_2.c

components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/my_lib_2.dir/my_lib_2.c.i"
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/components/my_lib_2/my_lib_2.c > CMakeFiles/my_lib_2.dir/my_lib_2.c.i

components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/my_lib_2.dir/my_lib_2.c.s"
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/components/my_lib_2/my_lib_2.c -o CMakeFiles/my_lib_2.dir/my_lib_2.c.s

# Object files for target my_lib_2
my_lib_2_OBJECTS = \
"CMakeFiles/my_lib_2.dir/my_lib_2.c.o"

# External object files for target my_lib_2
my_lib_2_EXTERNAL_OBJECTS =

components/my_lib_2/libmy_lib_2.a: components/my_lib_2/CMakeFiles/my_lib_2.dir/my_lib_2.c.o
components/my_lib_2/libmy_lib_2.a: components/my_lib_2/CMakeFiles/my_lib_2.dir/build.make
components/my_lib_2/libmy_lib_2.a: components/my_lib_2/CMakeFiles/my_lib_2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libmy_lib_2.a"
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && $(CMAKE_COMMAND) -P CMakeFiles/my_lib_2.dir/cmake_clean_target.cmake
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_lib_2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
components/my_lib_2/CMakeFiles/my_lib_2.dir/build: components/my_lib_2/libmy_lib_2.a
.PHONY : components/my_lib_2/CMakeFiles/my_lib_2.dir/build

components/my_lib_2/CMakeFiles/my_lib_2.dir/clean:
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 && $(CMAKE_COMMAND) -P CMakeFiles/my_lib_2.dir/cmake_clean.cmake
.PHONY : components/my_lib_2/CMakeFiles/my_lib_2.dir/clean

components/my_lib_2/CMakeFiles/my_lib_2.dir/depend:
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1 /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/components/my_lib_2 /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2 /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/components/my_lib_2/CMakeFiles/my_lib_2.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : components/my_lib_2/CMakeFiles/my_lib_2.dir/depend

