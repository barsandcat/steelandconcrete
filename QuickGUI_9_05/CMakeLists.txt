# Basic Variables
cmake_minimum_required(VERSION 2.4)
project(QuickGUI)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin" CACHE STRING "")
option(DEBUG FALSE)
if (DEBUG)
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/Debug" CACHE STRING "")
else()
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/Release" CACHE STRING "")
endif()

# Packages
find_package(Dependencies)
include_directories(${DEP_INCLUDE_PATH})
if (NOT OGRE_FOUND)
	message(FATAL_ERROR "OGRE not found!")
endif()
if (NOT OIS_FOUND)
	message(FATAL_ERROR "OIS not found!")
endif()

# Build Targets
add_subdirectory(QuickGUI)
add_subdirectory(QuickGUIEditor)

# Build Options
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	set_target_properties(QuickGUI PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

if (DEBUG)
	set(DEBUG_PROPERTIES "-g")
else()
	set(DEBUG_PROPERTIES "-w -s -Os -O3 -fexpensive-optimizations")
endif()
set_target_properties(QuickGUI PROPERTIES COMPILE_FLAGS ${DEBUG_PROPERTIES})
set_target_properties(QuickGUIEditor PROPERTIES COMPILE_FLAGS ${DEBUG_PROPERTIES})
