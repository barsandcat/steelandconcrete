#ifndef APP_H
#define APP_H

#include <OgreWindowCallback.h>
#include <GameState.h>
#include <ClientGeodesicGrid.h>
#include <ClientGame.h>

#include <OgreAL.h>
#include <BetaGUI.h>

class ClientApp
{
public:
    ClientApp(const Ogre::String aConfigFile);
    ~ClientApp();
    void MainLoop();

    void UpdateOISMouseClipping(Ogre::RenderWindow* rw);
    void DestroyOIS(Ogre::RenderWindow* rw);
    Ogre::Root& OgreRoot();
    Ogre::RenderWindow& Window();
    OgreAL::SoundManager& SoundManager();
    BetaGUI::GUI& Gui();
    void Quit();
    void SetState(GameState* aNewGameState);
    ClientGame* GetGame() const { return mGame; }
    OIS::Mouse* GetMouse() const { return mMouse; }
private:
    BetaGUI::GUI* mGUI;
    Ogre::OverlayContainer* mPointer;
    Ogre::Root* mRoot;
    Ogre::Plugin* mOctreePlugin;
    Ogre::Plugin* mGLPlugin;
    OgreWindowCallback* mWindowEventListener;
    OgreAL::SoundManager* mSoundManager;
    Ogre::RenderWindow* mWindow;
    bool mQuit;
    Ogre::Overlay* mDebugOverlay;
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;
    OIS::JoyStick* mJoy;
    GameState* mState;
    ClientGame* mGame;
    void ShowDebugOverlay(bool show);
    void UpdateStats();
};

#endif // APP_H
