#ifndef PCH_H
#define PCH_H

#include <Warnings.h>

// Disable all warnings for files icluded here
// For VC++
#pragma warning(push, 3)
// For GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wall"

// Disable some of windows includes to speedup build and avoid collision with glog ERROR
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0500
    #endif
    #ifndef _WIN32_WINDOWS
        #define _WIN32_WINDOWS 0x0500
    #endif
#endif

#include <boost/chrono/chrono.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include <OIS.h>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <OgreTextAreaOverlayElement.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <google/protobuf/stubs/common.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <Typedefs.h>
#include <Ogre.h>

// Restore warnings
#pragma warning(pop)
#pragma GCC diagnostic pop

#endif // PCH_H
