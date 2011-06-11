#ifndef APP_H
#define APP_H

#include <ClientGeodesicGrid.h>
#include <ClientGame.h>

#include <OgreAL.h>
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <QuickGUI.h>
#include <BirdCamera.h>

void LaunchServer();

class ClientApp: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener,
                 public Ogre::WindowEventListener
{
public:
    ClientApp(const Ogre::String aConfigFile);
    virtual ~ClientApp();
    void MainLoop();

    void OnClick(const QuickGUI::EventArgs& args);
    void OnConnect(const QuickGUI::EventArgs& args);
    void OnCreate(const QuickGUI::EventArgs& args);
    void OnBrowse(const QuickGUI::EventArgs& args);
    void OnRussian(const QuickGUI::EventArgs& args);
    void OnUkranian(const QuickGUI::EventArgs& args);
    void OnEnglish(const QuickGUI::EventArgs& args);
    void OnJapanese(const QuickGUI::EventArgs& args);
    void OnMainMenu(const QuickGUI::EventArgs& args);

    static Ogre::SceneManager& GetSceneMgr();
    static OgreAL::SoundManager& GetSoundMgr();
    static void Quit();
    static BirdCamera& GetCamera();
public:
    // OIS callbacks
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
    // Ogre call backs
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
private:
    static Ogre::SceneManager* mSceneMgr;
    static OgreAL::SoundManager* mSoundManager;
    static BirdCamera* mBirdCamera;
    static bool mQuit;
    static char RU[];
    static char EN[];
    static char UK[];
    static char JA[];

    CEGUI::OgreRenderer* mCEGUIRenderer;

    Ogre::Root* mRoot;
    Ogre::Plugin* mOctreePlugin;
    Ogre::Plugin* mGLPlugin;
    Ogre::RenderWindow* mWindow;
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;
    OIS::JoyStick* mJoy;

    ClientGame* mGame;
    boost::asio::io_service mIOService;
    boost::shared_ptr< boost::asio::io_service::work > mWork;
};

#endif // APP_H
