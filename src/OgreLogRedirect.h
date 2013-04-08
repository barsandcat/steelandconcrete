#ifndef OGRELOGREDIRECT_H
#define OGRELOGREDIRECT_H

#include <OgreLog.h>


class OgreLogRedirect : public Ogre::LogListener
{
public:
    virtual void messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml,
                               bool maskDebug, const Ogre::String &logName, bool& skipThisMessage);
};

#endif // OGRELOGREDIRECT_H
