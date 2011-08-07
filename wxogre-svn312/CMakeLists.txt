# wxOgre CMake toolchain,
#   Mostly inspired from OpenSceneGraph's one.
#   	see http://www.openscenegraph.org for more information.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Works around warnings libraries linked against that don't
# have absolute paths (e.g. -lOgreMain)
cmake_policy(SET CMP0003 NEW)

# Works around warnings about escaped quotes in ADD_DEFINITIONS
# statements.
cmake_policy(SET CMP0005 NEW)

cmake_policy(SET CMP0011 OLD)

PROJECT(wxOgre)

SET(WXOGRE_MAJOR_VERSION 0)
SET(WXOGRE_MINOR_VERSION 1)
SET(WXOGRE_PATCH_VERSION 0)

SET(WXOGRE_VERSION ${WXOGRE_MAJOR_VERSION}.${WXOGRE_MINOR_VERSION}.${WXOGRE_PATCH_VERSION})

SET(WXOGRE_SOVERSION ${WXOGRE_MAJOR_VERSION})
SET(WXOGRE_SONAMES TRUE)	# We want to build SONAMES shared librariess

SET(CMAKE_MODULE_PATH "${wxOgre_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

# Options
OPTION(BUILD_SHARED_LIBS "Set to OFF to build static libraries" ON)
OPTION(BUILD_EXAMPLES "Set to OFF to not build the examples" ON)

# Set default cmake build type to RelWithDebInfo
# (None Debug Release RelWithDebInfo MinSizeRel)
IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
ENDIF()

# Directories...
SET(WXOGRE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
SET(WXOGRE_TOOLS_BINARY_DIR ${WXOGRE_BINARY_DIR}/bin)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WXOGRE_TOOLS_BINARY_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${WXOGRE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${WXOGRE_BINARY_DIR}/lib)

#
# Yes we use some dependencies!
#
SET(wxWidgets_USE_LIBS base core)
FIND_PACKAGE(wxWidgets REQUIRED)
FIND_PACKAGE(OGRE REQUIRED)

IF(UNIX)
	# Here is the deal! We need to link against Gtk+
	# in order to use Gtk's internal macros to retrieve
	# the X11 winning trio: display, screen & window.
	FIND_PACKAGE(GTK2 REQUIRED)
ENDIF(UNIX)

# Include wxWidgets macros
INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES(${wxOgre_SOURCE_DIR}/include
		    ${OGRE_INCLUDE_DIR}
		    ${wxWidgets_INCLUDE_DIRS}
		    ${GTK2_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${OGRE_LIB_DIR}
		 ${wxWidgets_LIB_DIR}
)

# Where do you want to install your pkg-config file
SET(PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")

IF(CMAKE_COMPILER_IS_GNUCXX)
  # Turn off Ogre Warnings about deprecated headers includes...
  SET(CMAKE_CXX_FLAGS "-Wno-deprecated")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Common global definitions
#ADD_DEFINITIONS(-D)
# Platform specific definitions
IF(UNIX)
	# The OGRE_PLUGINDIR is used by the RenderSystem to load plugins
	# without using a special file (e.g. plugins.cfg). Under Windows
	# we assume that the DLL are in the folder of the binary.
	ADD_DEFINITIONS(-DOGRE_PLUGINDIR=\\"${OGRE_LIBDIR}/OGRE/\\")
	# Generate the pkg-config file
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/wxOgre.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/wxOgre.pc @ONLY)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/wxOgre.pc DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
ENDIF(UNIX)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)

# Uninstall target
CONFIGURE_FILE(
	"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY
)
ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Display status message for important variables
MESSAGE(STATUS)
MESSAGE(STATUS "-------------------------------------------------------------------------")
MESSAGE(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
MESSAGE(STATUS "BUILD_EXAMPLES = ${BUILD_EXAMPLES}")
MESSAGE(STATUS "Change a value with: cmake -D<Variable>=<Value>")
MESSAGE(STATUS "-------------------------------------------------------------------------")
MESSAGE(STATUS)
