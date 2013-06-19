#ifndef PCH_H
#define PCH_H

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

#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>

#include <google/protobuf/stubs/common.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <OIS.h>
#include <Ogre.h>

#include <Typedefs.h>
#include <ProtocolVersion.h>
#include <ReleaseVersion.h>

#include <Warnings.h>

// curses.h vs wincon.h macro collision. Currently there is no need in any, just to remove warning:
#undef MOUSE_MOVED

#endif // PCH_H
