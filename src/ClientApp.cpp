#include <pch.h>
#include <ClientApp.h>

#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <ServerProxy.h>
#include <ProtocolVersion.h>

#include <libintl.h>
#include <locale.h>

#include <RendererModules/Ogre/CEGUIOgreResourceProvider.h>
#include <RendererModules/Ogre/CEGUIOgreImageCodec.h>
#include <Platform.h>
#include <boost/asio/ssl.hpp>
#include <SSLLogRedirect.h>
#include <GUI.h>

char ClientApp::RU[] = "LANGUAGE=ru";
char ClientApp::EN[] = "LANGUAGE=en";
char ClientApp::UK[] = "LANGUAGE=uk";
char ClientApp::JA[] = "LANGUAGE=ja";

DEFINE_string(fsaa, "0", "FSAA samples count");
DEFINE_string(full_screen, "No", "Yes or No");
DEFINE_string(rtt_preferred_mode, "FBO", "FBO, Copy, PBuffer");
DEFINE_string(video_mode, "1024 x 768", "");
DEFINE_string(display_frequency, "60 MHz", "");
DEFINE_string(vsync, "No", "Yes or No");
DEFINE_string(srgb_gamma_conversion, "No", "Yes or No");
DEFINE_string(login, "test", "User name to use when connecting to server");
DEFINE_string(password, "test", "Password to use when connecting to server");
DEFINE_string(address, "127.0.0.1", "Server ip address");
DEFINE_string(port, "4512", "Server port");

static char* SSLGiveSRPClientPassword(SSL *s, void *arg)
{
    LOG(INFO) << "SSLGiveSRPClientPassword " << GetWindow("ServerBrowser/Password")->getText();
    return BUF_strdup(GetWindow("ServerBrowser/Password")->getText().c_str());
}

Ogre::SceneManager& ClientApp::GetSceneMgr()
{
    assert(ClientApp::mSceneMgr && "ClientApp::GetSceneMgr() \
        нельзя вызывать в конструктори и деструкторе ClientApp!");
    return *ClientApp::mSceneMgr;
}

OgreAL::SoundManager& ClientApp::GetSoundMgr()
{
    assert(ClientApp::mSoundManager && "ClientApp::GetSoundMgr() \
        нельзя вызывать в конструктори и деструкторе ClientApp!");
    return *ClientApp::mSoundManager;
}

Ogre::Camera& ClientApp::GetCamera()
{
    assert(mCamera && "ClientApp::GetCamera() \
        нельзя вызывать в конструкторе и деструкторе ClientApp!");
    return *mCamera;
}

OIS::Keyboard& ClientApp::GetKeyboard()
{
    assert(mKeyboard && "ClientApp::GetKeyboard() \
        нельзя вызывать в конструкторе и деструкторе ClientApp!");
    return *mKeyboard;
}

Ogre::SceneManager* ClientApp::mSceneMgr = NULL;
OgreAL::SoundManager* ClientApp::mSoundManager = NULL;
Ogre::Camera* ClientApp::mCamera = NULL;
OIS::Keyboard* ClientApp::mKeyboard = NULL;

ClientApp::ClientApp(int argc, char **argv):
    mCEGUIRenderer(NULL),
    mMouse(NULL),
    mGame(NULL),
    mSSLCtx(boost::asio::ssl::context::tlsv1_client)
{
    // No function calls allowed here - to avoid any checks for uninitialized members.

    Ogre::String config = GetFlagsFilePath();
    if (boost::filesystem::exists(config))
    {
        FLAGS_flagfile = config;
    }

    Ogre::String localConfig = "steelandconcrete.flags";
    if (boost::filesystem::exists(localConfig))
    {
        FLAGS_flagfile = localConfig;
    }

    google::ParseCommandLineFlags(&argc, &argv, true);

    // No logging before this call:
    google::InitGoogleLogging(argv[0]);

    {
        SSL_CTX* sslCtx = mSSLCtx.native_handle();
        SSL_CTX_set_info_callback(sslCtx, SSLInfoCallback);
        if (SSL_CTX_set_cipher_list(sslCtx, "SRP") != 1)
        {
            boost::throw_exception(std::runtime_error("SSL_CTX_set_cipher_list failed"));
        }
        SSL_CTX_set_verify(sslCtx, SSL_VERIFY_NONE, NULL);
    }

    mWork.reset(new boost::asio::io_service::work(mIOService));

    {
        LOG(INFO) << "Redirect Ogre log";
        Ogre::LogManager* logManager = new Ogre::LogManager();
        Ogre::Log* log = logManager->createLog("default.log", true, false, true);
        log->addListener(&mOgreLogRedirect);
    }

    {
        LOG(INFO) << "Init OGRE";
        mRoot = new Ogre::Root("", "", "");

        // Gl renedr system
        mGLPlugin = new Ogre::GLPlugin();
        mRoot->installPlugin(mGLPlugin);

        // Octree scene manager
        mOctreePlugin = new Ogre::OctreePlugin();
        mRoot->installPlugin(mOctreePlugin);


        Ogre::String data_dir(FLAGS_data_dir);
        LOG(INFO) << "Register resources in data_dir " << data_dir;
        Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
        rgm.addResourceLocation(data_dir + "/OgreCore", "FileSystem");
        rgm.addResourceLocation(data_dir + "/audio", "FileSystem");
        rgm.addResourceLocation(data_dir + "/textures", "FileSystem");
        rgm.addResourceLocation(data_dir + "/scripts", "FileSystem");
        rgm.addResourceLocation(data_dir + "/gui/fonts", "FileSystem");
        rgm.addResourceLocation(data_dir + "/gui/imagesets", "FileSystem");
        rgm.addResourceLocation(data_dir + "/gui/looknfeel", "FileSystem");
        rgm.addResourceLocation(data_dir + "/gui/schemes", "FileSystem");
        rgm.addResourceLocation(data_dir + "/gui/layouts", "FileSystem");

        boost::filesystem::directory_iterator end;
        boost::filesystem::path path(data_dir + "/models");
        for (boost::filesystem::directory_iterator i(path); i != end; ++i)
        {
            if (boost::filesystem::is_directory(i->status()))
            {
                rgm.addResourceLocation(i->path().string(), "FileSystem");
            }
        }

        Ogre::RenderSystem * renderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
        mRoot->setRenderSystem(renderSystem);

        renderSystem->setConfigOption("FSAA", FLAGS_fsaa);
        renderSystem->setConfigOption("Full Screen", FLAGS_full_screen);
        renderSystem->setConfigOption("RTT Preferred Mode", FLAGS_rtt_preferred_mode);
        renderSystem->setConfigOption("Video Mode", FLAGS_video_mode);
        renderSystem->setConfigOption("Display Frequency", FLAGS_display_frequency);
        renderSystem->setConfigOption("VSync", FLAGS_vsync);
        renderSystem->setConfigOption("sRGB Gamma Conversion", FLAGS_srgb_gamma_conversion);

        // Here we choose to let the system create a default rendering window by passing 'true'
        mRoot->initialise(false);
        mWindow = mRoot->createRenderWindow("Steel and concrete", 800, 600, false);
        // Set default mipmap level (NB some APIs ignore this)
        Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

        // Scene manager
        mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "EgoView");
        mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
        mSceneMgr->setAmbientLight(Ogre::ColourValue::White);
        LOG(INFO) << "=== Scene manager: " << mSceneMgr->getTypeName() << "===";

        // Create the camera
        mCamera = ClientApp::GetSceneMgr().createCamera("PlayerCam");
        mCamera->lookAt(Ogre::Vector3(0, 0, -300));
        mCamera->setNearClipDistance(0.01);
        // Create one viewport, entire window
        mWindow->removeAllViewports();
        mViewPort = mWindow->addViewport(mCamera);
        mViewPort->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()));
    }

    {
        LOG(INFO) << "Init OgreAL";
        mSoundManager = new OgreAL::SoundManager();
    }

    {
        LOG(INFO) << "Init localization";
        if (const char* env = getenv("LANGUAGE"))
        {
            LOG(INFO) << "LANGUAGE " << env;
        }
        LOG(INFO) << "locale " << setlocale(LC_ALL, "");
        LOG(INFO) << "bindtextdomain " << bindtextdomain("steelandconcrete", "lang");
        LOG(INFO) << "bind_textdomain_codeset " << bind_textdomain_codeset("steelandconcrete", "UTF-8");
        LOG(INFO) << "textdomain " << textdomain("steelandconcrete");
    }

    {
        LOG(INFO) << "Init OIS";
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));

        mInputManager = OIS::InputManager::createInputSystem(pl);

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
        mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

        //Set initial mouse clipping size
        unsigned int width, height, depth;
        int left, top;
        mWindow->getMetrics(width, height, depth, left, top);
        const OIS::MouseState &ms = mMouse->getMouseState();
        ms.width = width;
        ms.height = height;

        mMouse->setEventCallback(this);
        mKeyboard->setEventCallback(this);
    }

    {
        LOG(INFO) << "Init CEGUI";
        CEGUI::OgreRenderer& renderer = CEGUI::OgreRenderer::create(*mWindow);
        CEGUI::OgreResourceProvider* rp = &CEGUI::OgreRenderer::createOgreResourceProvider();
        CEGUI::OgreImageCodec* ic = &CEGUI::OgreRenderer::createOgreImageCodec();
        CEGUI::System::create(renderer, rp, static_cast<CEGUI::XMLParser*>(0), ic);
        CEGUI::FontManager::getSingleton().create("unifont.font");
        CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
        CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    }
#if OGRE_PROFILING
    Ogre::Profiler::getSingleton().setEnabled(true);
#endif
}

ClientApp::~ClientApp()
{
    LOG(INFO) << "App destructor";

    delete mGame;

    CEGUI::OgreRenderer::destroySystem();

    mSoundManager->destroyAllSounds();
    delete mSoundManager;
    mSoundManager = NULL;

    mRoot->destroySceneManager(mSceneMgr);
    mSceneMgr = NULL;

    mInputManager->destroyInputObject(mMouse);
    mInputManager->destroyInputObject(mKeyboard);

    OIS::InputManager::destroyInputSystem(mInputManager);
    mInputManager = 0;

    delete mRoot;
    mRoot = NULL;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);

    delete mOctreePlugin;
    mOctreePlugin = NULL;

    delete mGLPlugin;
    mGLPlugin = NULL;
}

void ClientApp::SubscribeToGUI()
{
    GetWindow("Main/Menu/English")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnEnglish, this));
    GetWindow("Main/Menu/Ukranian")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnUkranian, this));
    GetWindow("Main/Menu/Russian")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnRussian, this));
    GetWindow("Main/Menu/Japanese")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnJapanese, this));

    GetWindow("Main/Menu/Create")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnCreate, this));

    GetWindow("ServerBrowser/Connect")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnConnect, this));

    GetWindow("InGameMenu/DisconnectServer")->subscribeEvent(CEGUI::PushButton::EventClicked,
            CEGUI::Event::Subscriber(&ClientApp::OnDisconnect, this));
}

void ClientApp::ReloadGUI()
{
    LOG(INFO) << "Reload gui";
    CEGUI::WindowManager::getSingleton().destroyAllWindows();
    InitGUI();
    SubscribeToGUI();
}

void TriggerMsgCatalogReload()
{
    //http://www.gnu.org/software/gettext/manual/gettext.html#gettext-grok
    // ... The code for gcc-2.7.0 and up provides some optimization.
    // This optimization normally prevents the calling of the dcgettext
    // function as long as no new catalog is loaded. But if dcgettext is not
    // called the program also cannot find the LANGUAGE variable be changed
    textdomain("");
    textdomain("steelandconcrete");
}

bool ClientApp::OnRussian(const CEGUI::EventArgs& args)
{
    putenv(RU);
    TriggerMsgCatalogReload();
    ReloadGUI();
    return true;
}

bool ClientApp::OnEnglish(const CEGUI::EventArgs& args)
{
    putenv(EN);
    TriggerMsgCatalogReload();
    ReloadGUI();
    return true;
}

bool ClientApp::OnUkranian(const CEGUI::EventArgs& args)
{
    putenv(UK);
    TriggerMsgCatalogReload();
    ReloadGUI();
    return true;
}

bool ClientApp::OnJapanese(const CEGUI::EventArgs& args)
{
    putenv(JA);
    TriggerMsgCatalogReload();
    ReloadGUI();
    return true;
}

bool ClientApp::OnDisconnect(const CEGUI::EventArgs& args)
{
    delete mGame;
    mGame = 0;
    return true;
}

void ClientApp::OnAppHanshake(ServerProxyPtr aServerProxy, ConstPayloadPtr aRes)
{
    try
    {
        if (!aRes->has_avatar() || !aRes->has_size())
        {
            throw std::runtime_error(aRes->reason());
        }

        LOG(INFO) << "App handshake done. World size: " << aRes->size();

        mGame = new ClientGame(aServerProxy, aRes->avatar(), aRes->size());
    }
    catch (std::exception& e)
    {
        const char* what = e.what();
        LOG(ERROR) << "OnAppHandshake " << e.what();
        ShowMessageBox(what);
    }
}

void ClientApp::OnSSLHandShake(SSLStreamPtr aSSLStream, const boost::system::error_code& aError)
{
    try
    {
        if (aError)
        {
            boost::system::system_error e(aError);
            boost::throw_exception(e);
        }

        LOG(INFO) << "SSL handshake done";

        ServerProxyPtr serverProxy(new ServerProxy(aSSLStream));

        PayloadPtr req(new PayloadMsg());
        req->set_protocolversion(PROTOCOL_VERSION);
        serverProxy->Request(boost::bind(&ClientApp::OnAppHanshake, this, serverProxy, _1), req);
    }
    catch (std::exception& e)
    {
        const char* what = e.what();
        LOG(ERROR) << "OnSSLHandshake " << e.what();
        ShowMessageBox(what);
    }
}

void ClientApp::OnSocketConnect(SSLStreamPtr aSSLStream, const boost::system::error_code& aError)
{
    try
    {
        if (aError)
        {
            boost::system::system_error e(aError);
            boost::throw_exception(e);
        }
        LOG(INFO) << "Socket connected";

        aSSLStream->async_handshake(boost::asio::ssl::stream_base::client,
            boost::bind(&ClientApp::OnSSLHandShake, this, aSSLStream, boost::asio::placeholders::error));
    }
    catch (std::exception& e)
    {
        const char* what = e.what();
        LOG(ERROR) << "OnSocketConnect " << e.what();
        ShowMessageBox(what);
    }
}

bool ClientApp::OnConnect(const CEGUI::EventArgs& args)
{
    LOG(INFO) << "On connect";
    assert(!mGame);

    try
    {
        CEGUI::String port = GetWindow("ServerBrowser/Port")->getText();
        CEGUI::String address = GetWindow("ServerBrowser/Address")->getText();

        LOG(INFO) << "Port " << port << " Address " << address;

        boost::asio::ip::tcp::resolver resolver(mIOService);
        boost::asio::ip::tcp::resolver::query query(address.c_str(), port.c_str(),
            boost::asio::ip::resolver_query_base::numeric_service);
        boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);

        SSL_CTX* SSLCtx = mSSLCtx.native_handle();
        SSL_CTX_SRP_CTX_init(SSLCtx);

        char* login = const_cast<char*>(GetWindow("ServerBrowser/Login")->getText().c_str());
        LOG(INFO) << "SSL_CTX_set_srp_username " << login;

        if (SSL_CTX_set_srp_username(mSSLCtx.native_handle(), login) != 1)
        {
            boost::throw_exception(std::runtime_error("SSL_CTX_set_srp_username failed"));
        }

        SSL_CTX_set_srp_client_pwd_callback(SSLCtx, SSLGiveSRPClientPassword);

        SSLStreamPtr sslStream(new SSLStream(mIOService, mSSLCtx));

        boost::asio::async_connect(sslStream->lowest_layer(), endpointIterator,
            boost::bind(&ClientApp::OnSocketConnect, this, sslStream, boost::asio::placeholders::error));
    }
    catch (std::exception& e)
    {
        const char* what = e.what();
        LOG(ERROR) << "OnConnect " << e.what();
        ShowMessageBox(what);
    }
    return true;
}

bool ClientApp::OnCreate(const CEGUI::EventArgs& args)
{
    LOG(INFO) << "On create";
    if (!mGame)
    {
        LaunchServer();
    }
    return true;
}

bool ClientApp::keyPressed(const OIS::KeyEvent &arg)
{
    // do event injection
    CEGUI::System& cegui = CEGUI::System::getSingleton();
    if(cegui.injectKeyDown(arg.key) || cegui.injectChar(arg.text))
    {
        return true;
    }

    if (mGame)
    {
        mGame->keyPressed(arg);
    }

    return true;
}

bool ClientApp::keyReleased(const OIS::KeyEvent &arg)
{
    if (CEGUI::System::getSingleton().injectKeyUp(arg.key))
    {
        return true;
    }

    if (mGame)
    {
        mGame->keyReleased(arg);
    }

    return true;
}

bool ClientApp::mouseMoved(const OIS::MouseEvent &arg)
{
    CEGUI::System& cegui = CEGUI::System::getSingleton();

    cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
    cegui.injectMousePosition(arg.state.X.abs, arg.state.Y.abs);

    if (mGame)
    {
        mGame->mouseMoved(arg);
    }

    return true;
}

Ogre::Ray ClientApp::GetMouseRay() const
{
    const OIS::MouseState mouseState = mMouse->getMouseState();
    Ogre::Real aMouseX = Ogre::Real(mouseState.X.abs) / mouseState.width;
    Ogre::Real aMouseY = Ogre::Real(mouseState.Y.abs) / mouseState.height;
    Ogre::Ray ray;
    mCamera->getCameraToViewportRay(aMouseX, aMouseY, &ray);
    return ray;
}

CEGUI::MouseButton convertOISButtonToCegui(int buttonID)
{
    using namespace OIS;

    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
    case OIS::MB_Right:
        return CEGUI::RightButton;
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
    default:
        return CEGUI::LeftButton;
    }
}


bool ClientApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if (CEGUI::System::getSingleton().injectMouseButtonDown(convertOISButtonToCegui(id)))
    {
        return true;
    }

    if (mGame)
    {
        mGame->mousePressed(arg, id);
    }

    return true;
}

bool ClientApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonUp(convertOISButtonToCegui(id));
    if (mGame)
    {
        mGame->mouseReleased(arg, id);
    }
    return true;
}

void ClientApp::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

    CEGUI::Size size(static_cast<float>(width), static_cast<float>(height));
    CEGUI::System::getSingleton().notifyDisplaySizeChanged(size);
}

void ClientApp::windowClosed(Ogre::RenderWindow* rw)
{
    boost::throw_exception(std::runtime_error("Window closed"));
}

Ogre::Real FrameTimeToSeconds(unsigned long aFrimeTime)
{
    return aFrimeTime / 1000000.0f;
}

void ClientApp::MainLoop()
{
    ReloadGUI();

    unsigned long frameTime = 1;

    LOG(INFO) << "*** The Start ***";
    try
    {
        while(true)
        {
            OgreProfile("Ogre Main Loop");
            FrameTime frameStart = mRoot->getTimer()->getMicroseconds();

            {
                OgreProfile("Update");
                Ogre::WindowEventUtilities::messagePump();
                CEGUI::System::getSingleton().injectTimePulse(FrameTimeToSeconds(frameTime));


                mKeyboard->capture();
                mMouse->capture();

                if (mGame)
                {
                    mGame->UpdateTileUnderCursor(GetMouseRay());
                    mGame->Update(frameTime, mWindow->getStatistics());
                }

                mIOService.poll();
            }
            mRoot->renderOneFrame();

            frameTime = mRoot->getTimer()->getMicroseconds() - frameStart;
        }
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "Main loop exception:" << e.what();
    }

    LOG(INFO) << "*** The End ***";

    // Подчищаем игру
    delete mGame;
    mGame = NULL;
}
