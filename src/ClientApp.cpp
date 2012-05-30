#include <pch.h>
#include <ClientApp.h>

#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <Network.h>
#include <ProtocolVersion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <libintl.h>
#include <locale.h>

#include <RendererModules/Ogre/CEGUIOgreResourceProvider.h>
#include <RendererModules/Ogre/CEGUIOgreImageCodec.h>
#include <CEGUILocalization.h>
#include <Platform.h>

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

CEGUI::Window* GetWindow(CEGUI::String aWindowName)
{
    return CEGUI::WindowManager::getSingleton().getWindow(aWindowName);
}

void ShowModal(CEGUI::String aWindowName)
{
    CEGUI::Window* window = GetWindow(aWindowName);
    window->setModalState(true);
    window->setVisible(true);
    window->setAlwaysOnTop(true);
}

void HideModal(CEGUI::String aWindowName)
{
    CEGUI::Window* window = GetWindow(aWindowName);
    window->setModalState(false);
    window->setVisible(false);
    window->setAlwaysOnTop(false);
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

Ogre::Camera* ClientApp::GetCamera()
{
    assert(mCamera && "ClientApp::GetCamera() \
        нельзя вызывать в конструкторе и деструкторе ClientApp!");
    return mCamera;
}

void ClientApp::Quit()
{
    mQuit = true;
}

Ogre::SceneManager* ClientApp::mSceneMgr = NULL;
OgreAL::SoundManager* ClientApp::mSoundManager = NULL;
Ogre::Camera* ClientApp::mCamera = NULL;

bool ClientApp::mQuit = false;

ClientApp::ClientApp(int argc, char **argv):
    mCEGUIRenderer(NULL),
    mMouse(NULL),
    mKeyboard(NULL),
    mGame(NULL)
{
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
    google::InitGoogleLogging(argv[0]);

    // Только для инициализации!
    // Ни каких вызовов других функий этого класа, что бы небыло необходимости
    // проверять поля на то что они инициализированы.
    {
        LOG(INFO) << "Redirect Ogre log";
        Ogre::LogManager* logManager = new Ogre::LogManager();
        Ogre::Log* log = logManager->createLog("default.log", true, false, true);
        log->addListener(&mOgreLogRedirect);
    }

    {
        mWork.reset(new boost::asio::io_service::work(mIOService));

        mRoot = new Ogre::Root("", "", "");
        LOG(INFO) << "Init OGRE";

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
#ifdef MOUSE_GRAB
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("true")));
#else
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
#endif
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

        CEGUI::GlobalEventSet::getSingleton().subscribeEvent("PushButton/Clicked",
                CEGUI::Event::Subscriber(&ClientApp::OnClick, this));
    }
#if OGRE_PROFILING
    Ogre::Profiler::getSingleton().setEnabled(true);
#endif
}

ClientApp::~ClientApp()
{
    // Зачистка системных библиотек.
    // ТОЛЬКО ДЛЯ ТОГО ЧТО БЫЛО ИНИЦИАЛИЗОРВАНО В КОНСТРУКТОЕ
    // все остальное должно быть уже почищено!
    LOG(INFO) << "App destructor";

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

    google::ShutdownGoogleLogging();
}

void ClientApp::BuildMainGUILayout()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.destroyAllWindows();
    CEGUI::Window* myRoot = winMgr.loadWindowLayout("Main.layout", "", "", &PropertyCallback);
    CEGUI::System::getSingleton().setGUISheet(myRoot);
    GetWindow("ServerBrowser/Port")->setText("4512");
    GetWindow("ServerBrowser/Address")->setText("127.0.0.1");

    GetWindow("MainMenu/English")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnEnglish, this));
    GetWindow("MainMenu/Ukranian")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnUkranian, this));
    GetWindow("MainMenu/Russian")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnRussian, this));
    GetWindow("MainMenu/Japanese")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnJapanese, this));

    GetWindow("MainMenu/Create")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnCreate, this));
    GetWindow("MainMenu/Connect")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnBrowse, this));

    GetWindow("ServerBrowser/Connect")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnConnect, this));
    GetWindow("ServerBrowser/Cancel")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnMainMenu, this));
    GetWindow("MessageBox/Close")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientApp::OnCloseMessageBox, this));
}

bool ClientApp::OnClick(const CEGUI::EventArgs& args)
{
    OgreAL::Sound *sound = NULL;
    if (mSoundManager->hasSound("click"))
    {
        sound = mSoundManager->getSound("click");
        sound->stop();
    }
    else
    {
        sound = mSoundManager->createSound("click", "clickclick.ogg", false, false);
        sound->setRelativeToListener(true);
    }
    sound->play();
    return true;
}

bool ClientApp::OnBrowse(const CEGUI::EventArgs& args)
{
    ShowModal("ServerBrowser");
    return true;
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
    BuildMainGUILayout();
    return true;
}

bool ClientApp::OnEnglish(const CEGUI::EventArgs& args)
{
    putenv(EN);
    TriggerMsgCatalogReload();
    BuildMainGUILayout();
    return true;
}

bool ClientApp::OnUkranian(const CEGUI::EventArgs& args)
{
    putenv(UK);
    TriggerMsgCatalogReload();
    BuildMainGUILayout();
    return true;
}

bool ClientApp::OnJapanese(const CEGUI::EventArgs& args)
{
    putenv(JA);
    TriggerMsgCatalogReload();
    BuildMainGUILayout();
    return true;
}

bool ClientApp::OnMainMenu(const CEGUI::EventArgs& args)
{
    HideModal("ServerBrowser");
    return true;
}

bool ClientApp::OnCloseMessageBox(const CEGUI::EventArgs& args)
{
    HideModal("MessageBox");
    return true;
}


bool ClientApp::OnConnect(const CEGUI::EventArgs& args)
{
    LOG(INFO) << "On connect";
    assert(!mGame);

    try
    {
        CEGUI::String port = GetWindow("ServerBrowser/Port")->getText();
        CEGUI::String address = GetWindow("ServerBrowser/Address")->getText();

        LOG(INFO) << "Port " << port << "Address " << address;

        tcp::resolver resolver(mIOService);
        tcp::resolver::query query(address.c_str(), port.c_str(), boost::asio::ip::resolver_query_base::numeric_service);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        SocketSharedPtr sock(new tcp::socket(mIOService));
        sock->connect(*iterator);

        NetworkPtr net(new Network(sock));
        LOG(INFO) << "Connected";

        PayloadMsg req;
        req.set_protocolversion(PROTOCOL_VERSION);
        net->WriteMessage(req);

        PayloadMsg res;
        net->ReadMessage(res);
        if (!res.has_avatar() || !res.has_size())
        {
            throw std::runtime_error(res.reason());
        }

        mGame = new ClientGame(net, res.avatar(), res.size());
        GetWindow("MainMenu")->setVisible(false);
        HideModal("ServerBrowser");
    }
    catch (std::exception& e)
    {
        const char* what = e.what();
        GetWindow("MessageBox/Message")->setText(what);
        ShowModal("MessageBox");
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
    mQuit = true;
    LOG(INFO) << "Window is closed";
}


void ClientApp::MainLoop()
{
    BuildMainGUILayout();

    unsigned long frameTime = 1;

    LOG(INFO) << "*** The Start ***";
    while (!mQuit)
    {
        OgreProfile("Ogre Main Loop");
        FrameTime frameStart = mRoot->getTimer()->getMicroseconds();

        {
            OgreProfile("Update");
            Ogre::WindowEventUtilities::messagePump();
            CEGUI::System::getSingleton().injectTimePulse(frameTime / 1000000.0f);


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
    LOG(INFO) << "*** The End ***";

    // Подчищаем игру
    delete mGame;
    mGame = NULL;
}
