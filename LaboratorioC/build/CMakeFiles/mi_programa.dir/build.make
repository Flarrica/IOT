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
include CMakeFiles/mi_programa.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mi_programa.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mi_programa.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mi_programa.dir/flags.make

CMakeFiles/mi_programa.dir/codegen:
.PHONY : CMakeFiles/mi_programa.dir/codegen

CMakeFiles/mi_programa.dir/src/main.c.o: CMakeFiles/mi_programa.dir/flags.make
CMakeFiles/mi_programa.dir/src/main.c.o: /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/src/main.c
CMakeFiles/mi_programa.dir/src/main.c.o: CMakeFiles/mi_programa.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/mi_programa.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/mi_programa.dir/src/main.c.o -MF CMakeFiles/mi_programa.dir/src/main.c.o.d -o CMakeFiles/mi_programa.dir/src/main.c.o -c /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/src/main.c

CMakeFiles/mi_programa.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/mi_programa.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/src/main.c > CMakeFiles/mi_programa.dir/src/main.c.i

CMakeFiles/mi_programa.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/mi_programa.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/src/main.c -o CMakeFiles/mi_programa.dir/src/main.c.s

# Object files for target mi_programa
mi_programa_OBJECTS = \
"CMakeFiles/mi_programa.dir/src/main.c.o"

# External object files for target mi_programa
mi_programa_EXTERNAL_OBJECTS =

mi_programa: CMakeFiles/mi_programa.dir/src/main.c.o
mi_programa: CMakeFiles/mi_programa.dir/build.make
mi_programa: components/Global/libGlobal.a
mi_programa: components/my_lib_1_1/libmy_lib_1_1.a
mi_programa: components/my_lib_1_2/libmy_lib_1_2.a
mi_programa: components/my_lib_2/libmy_lib_2.a
mi_programa: CMakeFiles/mi_programa.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable mi_programa"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mi_programa.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mi_programa.dir/build: mi_programa
.PHONY : CMakeFiles/mi_programa.dir/build

CMakeFiles/mi_programa.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mi_programa.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mi_programa.dir/clean

CMakeFiles/mi_programa.dir/depend:
	cd /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1 /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1 /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build /Users/marcosgiombini/Documents/GitHub/IOT/Laboratorio1/build/CMakeFiles/mi_programa.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/mi_programa.dir/depend

