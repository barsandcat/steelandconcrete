#include <pch.h>
#include <OgreLogRedirect.h>

void OgreLogRedirect::messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml,
                                            bool maskDebug, const Ogre::String &logName )
{
    switch(lml)
    {
        case Ogre::LML_CRITICAL:
            LOG(ERROR) << message;
            break;
        default:
            LOG(INFO) << message;
    }
}
