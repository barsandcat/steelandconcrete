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
#include <OgreAL.h>
#include <QuickGUI.h>

#ifdef MVC
#pragma warning(pop)
#endif

#include <BetaGUI.h>
#include <stdexcept>

#endif // PCH_H
