# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = X:\CMake\bin\cmake.exe

# The command to remove a file.
RM = X:\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = X:\Coding\CPP\mp_os

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = X:\Coding\CPP\mp_os\build

# Include any dependencies generated for this target.
include associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/compiler_depend.make

# Include the progress variables for this target.
include associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/progress.make

# Include the compile flags for this target's objects.
include associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/flags.make

# Object files for target mp_os_assctv_cntnr
mp_os_assctv_cntnr_OBJECTS =

# External object files for target mp_os_assctv_cntnr
mp_os_assctv_cntnr_EXTERNAL_OBJECTS =

associative_container/libmp_os_assctv_cntnr.a: associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/build.make
associative_container/libmp_os_assctv_cntnr.a: associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=X:\Coding\CPP\mp_os\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX static library libmp_os_assctv_cntnr.a"
	cd /d X:\Coding\CPP\mp_os\build\associative_container && $(CMAKE_COMMAND) -P CMakeFiles\mp_os_assctv_cntnr.dir\cmake_clean_target.cmake
	cd /d X:\Coding\CPP\mp_os\build\associative_container && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\mp_os_assctv_cntnr.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/build: associative_container/libmp_os_assctv_cntnr.a
.PHONY : associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/build

associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/clean:
	cd /d X:\Coding\CPP\mp_os\build\associative_container && $(CMAKE_COMMAND) -P CMakeFiles\mp_os_assctv_cntnr.dir\cmake_clean.cmake
.PHONY : associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/clean

associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" X:\Coding\CPP\mp_os X:\Coding\CPP\mp_os\associative_container X:\Coding\CPP\mp_os\build X:\Coding\CPP\mp_os\build\associative_container X:\Coding\CPP\mp_os\build\associative_container\CMakeFiles\mp_os_assctv_cntnr.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : associative_container/CMakeFiles/mp_os_assctv_cntnr.dir/depend

