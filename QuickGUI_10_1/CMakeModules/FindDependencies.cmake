# Set Search Paths
if (WIN32)
elseif (UNIX)
	set(DEP_INCLUDE_PATH
		/usr/include
		/usr/include/OGRE
		/usr/include/OIS
		/usr/local/include
		/usr/local/include/OGRE
		/usr/local/include/OIS
	)
	set(DEP_LIB_SEARCH_DIR
		/usr/lib
		/usr/local/lib
	)
	set(DEP_LIBD_SEARCH_DIR ${DEP_LIB_SEARCH_DIR})
	include_directories(DEP_INCLUDE_PATH)
elseif (APPLE)
endif()

# Package Config
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(PKGDEP OGRE OIS)
	if (PKGDEP_FOUND)
		set(DEP_INCLUDE_PATH ${DEP_INCLUDE_PATH} ${PKGDEP_INCLUDE_PATHS})
		set(DEP_LIB_SEARCH_DIR ${DEP_LIB_SEARCH_DIR} ${PKGDEP_LIBRARY_DIRS})
		set(OGRE_FOUND TRUE)
		set(OIS_FOUND TRUE)
	endif()
endif()

# Find OGRE
find_path(OGRE_INCLUDE_DIR Ogre.h ${DEP_INCLUDE_PATH})
find_library(OGRE_LIBRARY_REL NAMES OgreMain PATHS ${DEP_LIB_SEARCH_DIR})
find_library(OGRE_LIBRARY_DBG NAMES OgreMain_d PATHS ${DEP_LIBD_SEARCH_DIR})
if (OGRE_LIBRARY_REL AND OGRE_LIBRARY_DBG)
	set(OGRE_LIBRARY optimized ${OGRE_LIBRARY_REL} debug ${OGRE_LIBRARY_DBG})
elseif (OGRE_LIBRARY_REL)
	set(OGRE_LIBRARY ${OGRE_LIBRARY_REL})
elseif (OGRE_LIBRARY_DBG)
	set(OGRE_LIBRARY ${OGRE_LIBRARY_DBG})
endif()	
if (OGRE_INCLUDE_DIR AND OGRE_LIBRARY)
	set(OGRE_FOUND TRUE)
endif()

# Find OIS
find_path(OIS_INCLUDE_DIR OIS.h ${DEP_INCLUDE_PATH})
find_library(OIS_LIBRARY_REL NAMES OIS PATHS ${DEP_LIB_SEARCH_DIR})
find_library(OIS_LIBRARY_DBG NAMES OIS_d PATHS ${DEP_LIBD_SEARCH_DIR})
if (OIS_LIBRARY_REL AND OIS_LIBRARY_DBG)
	set(OIS_LIBRARY optimized ${OIS_LIBRARY_REL} debug ${OIS_LIBRARY_DBG})
elseif (OIS_LIBRARY_REL)
	set(OIS_LIBRARY ${OIS_LIBRARY_REL})
elseif (OIS_LIBRARY_DBG)
	set(OIS_LIBRARY ${OIS_LIBRARY_DBG})
endif()	
if (OIS_INCLUDE_DIR AND OIS_LIBRARY)
	set(OIS_FOUND TRUE)
endif()
