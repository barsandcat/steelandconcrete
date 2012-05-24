#ifndef PCH_H
#define PCH_H

// Disable warnings for files icluded here
#ifdef _MSC_VER
    #pragma warning(push, 3)
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wreturn-type"
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wall"
#endif


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

#include <OIS.h>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/thread.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <OgreTextAreaOverlayElement.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <google/protobuf/stubs/common.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <Typedefs.h>
#include <Ogre.h>


#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)
    #pragma GCC diagnostic pop
#endif

#endif // PCH_H
