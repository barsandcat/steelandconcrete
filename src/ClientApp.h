#ifndef APP_H
#define APP_H

#include <OgreWindowCallback.h>
#include <ClientGeodesicGrid.h>
#include <ClientGame.h>

#include <OgreAL.h>
#include <QuickGUI.h>
#include <BirdCamera.h>
#include <MainMenuSheet.h>
#include <ServerBrowserSheet.h>

void LaunchServer();

class ClientApp: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
    ClientApp(const Ogre::String aConfigFile);
    virtual ~ClientApp();
    void MainLoop();
    void UpdateOISMouseClipping(Ogre::RenderWindow* rw);
    void UpdateSheetSize(Ogre::RenderWindow* rw);
    void DestroyOIS(Ogre::RenderWindow* rw);

    void OnConnect(const QuickGUI::EventArgs& args);
    void OnCreate(const QuickGUI::EventArgs& args);
    void OnBrowse(const QuickGUI::EventArgs& args);
    void OnMainMenu(const QuickGUI::EventArgs& args);

    virtual bool buttonPressed(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    virtual bool buttonReleased(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis)
    {
        return true;
    }
    virtual bool mouseMoved(const OIS::MouseEvent& arg);
    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool keyPressed(const OIS::KeyEvent& arg);
    virtual bool keyReleased(const OIS::KeyEvent& arg);

    static QuickGUI::GUIManager& GetGuiMgr();
    static Ogre::SceneManager& GetSceneMgr();
    static OgreAL::SoundManager& GetSoundMgr();
    static void Quit();
    static BirdCamera& GetCamera();
private:
    static QuickGUI::GUIManager* mGUIManager;
    static Ogre::SceneManager* mSceneMgr;
    static OgreAL::SoundManager* mSoundManager;
    static BirdCamera* mBirdCamera;
    static bool mQuit;

    Ogre::OverlayContainer* mPointer;
    Ogre::Root* mRoot;
    Ogre::Plugin* mOctreePlugin;
    Ogre::Plugin* mGLPlugin;
    OgreWindowCallback* mWindowEventListener;
    Ogre::RenderWindow* mWindow;
    Ogre::Overlay* mDebugOverlay;
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;
    OIS::JoyStick* mJoy;

    ClientGame* mGame;
    MainMenuSheet* mMainMenu;
    ServerBrowserSheet* mServerBrowserSheet;
    void ShowDebugOverlay(bool show);
    void UpdateStats();
    void Frame(unsigned long aFrameTime);
};

#endif // APP_H
