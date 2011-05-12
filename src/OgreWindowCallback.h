#ifndef OGREWINDOWCALLBACK_H
#define OGREWINDOWCALLBACK_H

#include <OgreWindowEventUtilities.h>

class ClientApp;

class OgreWindowCallback: public Ogre::WindowEventListener
{
public:
    OgreWindowCallback(ClientApp & aApp);
    ~OgreWindowCallback(void);

    //Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);

    //Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

private:
    ClientApp & mApp;
};

#endif //OGREWINDOWCALLBACK_H
