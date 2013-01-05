#ifndef PLATFORM_H
#define PLATFORM_H

#include <Typedefs.h>

#include <OgreString.h>
#include <gflags/gflags.h>

DECLARE_string(data_dir);

Ogre::String GetFlagsFilePath();
void LaunchServer();

#endif
