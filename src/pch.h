#ifndef PCH_H
#define PCH_H

// Отключаем варнинги для MVC++
#ifdef _MSC_VER
    #pragma warning(push, 3)
#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0500
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

#include <glog/logging.h>

#include <Typedefs.h>
#include <Ogre.h>


#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#endif // PCH_H
