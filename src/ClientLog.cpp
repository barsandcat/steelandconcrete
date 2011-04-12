#include <pch.h>
#include <ClientLog.h>

Ogre::Log::Stream GetLog()
{
    return  Ogre::LogManager::getSingleton().stream();
}
