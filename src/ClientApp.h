#ifndef APP_H
#define APP_H

#include <OgreWindowCallback.h>
#include <ClientGeodesicGrid.h>
#include <ClientGame.h>

#include <OgreAL.h>
#include <QuickGUI.h>
#include <BirdCamera.h>


class ClientApp: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
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
    void Quit();
    ClientGame* GetGame() const { return mGame; }
    OIS::Mouse* GetMouse() const { return mMouse; }
    void Frame(unsigned long aFrameTime);
    bool buttonPressed(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    bool buttonReleased(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    bool axisMoved(const OIS::JoyStickEvent &arg, int axis)
    {
        return true;
    }
    bool mouseMoved(const OIS::MouseEvent& arg);
    bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool keyPressed(const OIS::KeyEvent& arg);
    bool keyReleased(const OIS::KeyEvent& arg);
private:
    QuickGUI::GUIManager* mGUIManager;
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

    ClientGame* mGame;
    BirdCamera* mBirdCamera;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneManager* mSceneMgr;
    void EgoView();
    void DeleteEgoView();
    void UpdateSelectedTilePosition(const OIS::MouseState &aState);
    void ShowDebugOverlay(bool show);
    void UpdateStats();
};

#endif // APP_H
