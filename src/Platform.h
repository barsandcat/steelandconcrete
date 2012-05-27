#ifndef PLATFORM_H
#define PLATFORM_H

#include <Typedefs.h>

#include <OgreString.h>

int64 GetMiliseconds();

void CheckConfigFile(const Ogre::String aHomeDir);

void LaunchServer();


#endif
