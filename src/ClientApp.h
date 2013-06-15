#ifndef APP_H
#define APP_H

#include <ClientGeodesicGrid.h>
#include <ClientGame.h>

#include <OgreAL.h>
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <CEGUILogRedirect.h>
#include <OgreLogRedirect.h>

DECLARE_string(login);
DECLARE_string(password);
DECLARE_string(address);
DECLARE_string(port);

float FrameTimeToSeconds(Miliseconds aFrimeTime);

class ClientApp: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener,
                 public Ogre::WindowEventListener
{
public:
    ClientApp(int argc, char **argv);
    virtual ~ClientApp();
    void MainLoop();

    static Ogre::SceneManager& GetSceneMgr();
    static OgreAL::SoundManager& GetSoundMgr();
    static Ogre::Camera& GetCamera();
    static OIS::Keyboard& GetKeyboard();
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
    bool OnClick(const CEGUI::EventArgs& args);
    bool OnConnect(const CEGUI::EventArgs& args);
    bool OnCreate(const CEGUI::EventArgs& args);
    bool OnRussian(const CEGUI::EventArgs& args);
    bool OnUkranian(const CEGUI::EventArgs& args);
    bool OnEnglish(const CEGUI::EventArgs& args);
    bool OnJapanese(const CEGUI::EventArgs& args);
    bool OnDisconnect(const CEGUI::EventArgs& args);
    void OnAppHanshake(ServerProxyPtr aServerProxy, ConstPayloadPtr aRes);
    void OnSSLHandShake(SSLStreamPtr aSSLStream, const boost::system::error_code& aError);
    void OnSocketConnect(SSLStreamPtr aSSLStream, const boost::system::error_code& aError);
    void ReloadGUI();
    void SubscribeToGUI();
    Ogre::Ray GetMouseRay() const;
    static Ogre::SceneManager* mSceneMgr;
    static OgreAL::SoundManager* mSoundManager;
    static Ogre::Camera* mCamera;
    static OIS::Keyboard* mKeyboard;
    static char RU[];
    static char EN[];
    static char UK[];
    static char JA[];

    CEGUI::OgreRenderer* mCEGUIRenderer;

    Ogre::Root* mRoot;
    Ogre::Plugin* mOctreePlugin;
    Ogre::Plugin* mGLPlugin;
    Ogre::RenderWindow* mWindow;
    Ogre::Viewport* mViewPort;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;


    ClientGame* mGame;
    boost::asio::io_service mIOService;
    boost::shared_ptr< boost::asio::io_service::work > mWork;
    boost::asio::ssl::context mSSLCtx;
    OgreLogRedirect mOgreLogRedirect;
    CEGUILogRedirect mCEGUILogRedirect;
};

#endif // APP_H
