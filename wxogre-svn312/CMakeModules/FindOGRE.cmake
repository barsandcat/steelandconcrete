# Find OGRE includes and library
#
# This module defines
#  OGRE_INCLUDE_DIR
#  OGRE_LIBRARIES, the libraries to link against to use OGRE.
#  OGRE_LIB_DIR, the location of the libraries
#  OGRE_FOUND, If false, do not try to use OGRE
#
# Copyright © 2007, Matt Williams
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products 
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

IF (OGRE_LIBRARIES AND OGRE_INCLUDE_DIR)
	SET(OGRE_FIND_QUIETLY TRUE) # Already in cache, be silent
ENDIF (OGRE_LIBRARIES AND OGRE_INCLUDE_DIR)

IF (WIN32) #Windows
	MESSAGE(STATUS "Looking for OGRE")
	SET(OGRESDK $ENV{OGRE_HOME})
	SET(OGRESOURCE $ENV{OGRE_SRC})
	IF (OGRESDK)
		MESSAGE(STATUS "Using OGRE SDK")
		STRING(REGEX REPLACE "[\\]" "/" OGRESDK "${OGRESDK}")
		SET(OGRE_INCLUDE_DIR ${OGRESDK}/include)
		SET(OGRE_LIB_DIR ${OGRESDK}/lib)
		SET(OGRE_LIBRARIES debug OgreMain_d optimized OgreMain)
	ENDIF (OGRESDK)
	IF (OGRESOURCE)
		MESSAGE(STATUS "Using OGRE built from source")
		SET(OGRE_INCLUDE_DIR $ENV{OGRE_SRC}/OgreMain/include)
		SET(OGRE_LIB_DIR $ENV{OGRE_SRC}/lib)
		SET(OGRE_LIBRARIES debug OgreMain_d optimized OgreMain)
	ENDIF (OGRESOURCE)
ELSE (WIN32) #Unix
	CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
	FIND_PACKAGE(PkgConfig)
	PKG_SEARCH_MODULE(OGRE OGRE)
	SET(OGRE_INCLUDE_DIR ${OGRE_INCLUDE_DIRS})
	SET(OGRE_LIB_DIR ${OGRE_LIBDIR})
	SET(OGRE_LIBRARIES ${OGRE_LIBRARIES} CACHE STRING "")
ENDIF (WIN32)

#Do some preparation
SEPARATE_ARGUMENTS(OGRE_INCLUDE_DIR)
SEPARATE_ARGUMENTS(OGRE_LIBRARIES)

SET(OGRE_INCLUDE_DIR ${OGRE_INCLUDE_DIR} CACHE PATH "")
SET(OGRE_LIBRARIES ${OGRE_LIBRARIES} CACHE STRING "")
SET(OGRE_LIB_DIR ${OGRE_LIB_DIR} CACHE PATH "")

IF (OGRE_INCLUDE_DIR AND OGRE_LIBRARIES)
	SET(OGRE_FOUND TRUE)
ENDIF (OGRE_INCLUDE_DIR AND OGRE_LIBRARIES)

IF (OGRE_FOUND)
	IF (NOT OGRE_FIND_QUIETLY)
		MESSAGE(STATUS "  libraries : ${OGRE_LIBRARIES} from ${OGRE_LIB_DIR}")
		MESSAGE(STATUS "  includes  : ${OGRE_INCLUDE_DIR}")
	ENDIF (NOT OGRE_FIND_QUIETLY)
ELSE (OGRE_FOUND)
	IF (OGRE_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find OGRE")
	ENDIF (OGRE_FIND_REQUIRED)
ENDIF (OGRE_FOUND)
