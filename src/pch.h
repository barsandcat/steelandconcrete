#ifndef PCH_H
#define PCH_H

// Отключаем варнинги для MVC++
#ifdef MVC
#pragma warning(push, 3)
#endif

#include <OIS.h>
#include <Ogre.h>
#include <OgreFontManager.h>
#include <OgreFont.h>
#include <OgreTextAreaOverlayElement.h>
#include <Typedefs.h>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <google/protobuf/stubs/common.h>
#include <boost/thread.hpp>
#include <boost/exception/diagnostic_information.hpp>


#ifdef MVC
#pragma warning(pop)
#endif

#endif // PCH_H
