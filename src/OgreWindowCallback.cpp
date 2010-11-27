#include <pch.h>
#include <OgreWindowCallback.h>
#include <ClientApp.h>

OgreWindowCallback::OgreWindowCallback(ClientApp & aApp): mApp(aApp)
{
}

OgreWindowCallback::~OgreWindowCallback(void)
{
}

//Adjust mouse clipping area
void OgreWindowCallback::windowResized(Ogre::RenderWindow* rw)
{
    mApp.UpdateOISMouseClipping(rw);
    mApp.UpdateSheetSize(rw);
}

//Unattach OIS before window shutdown (very important under Linux)
void OgreWindowCallback::windowClosed(Ogre::RenderWindow* rw)
{
    mApp.DestroyOIS(rw);
}
