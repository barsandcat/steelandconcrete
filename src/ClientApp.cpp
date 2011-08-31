#include <pch.h>
#include <ClientApp.h>

#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <Network.h>
#include <ClientLog.h>
#include <ProtocolVersion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <libintl.h>
#include <locale.h>

#include <RendererModules/Ogre/CEGUIOgreResourceProvider.h>
#include <RendererModules/Ogre/CEGUIOgreImageCodec.h>
#include <CEGUILocalization.h>

char ClientApp::RU[] = "LANGUAGE=ru";
char ClientApp::EN[] = "LANGUAGE=en";
char ClientApp::UK[] = "LANGUAGE=uk";
char ClientApp::JA[] = "LANGUAGE=ja";

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

ClientApp::ClientApp(const Ogre::String aConfigFile):
    mCEGUIRenderer(NULL),
    mMouse(NULL),
    mKeyboard(NULL),
    mGame(NULL)
{
    // Только для инициализации!
    // Ни каких вызовов других функий этого класа, что бы небыло необходимости
    // проверять поля на то что они инициализированы.
    {
        mWork.reset(new boost::asio::io_service::work(mIOService));

        mRoot = new Ogre::Root("", "", "Ogre.log");
        GetLog() << "Init OGRE";

        // Gl renedr system
        mGLPlugin = new Ogre::GLPlugin();
        mRoot->installPlugin(mGLPlugin);

        // Octree scene manager
        mOctreePlugin = new Ogre::OctreePlugin();
        mRoot->installPlugin(mOctreePlugin);

        // Register resources
        Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
        rgm.addResourceLocation("res/OgreCore", "FileSystem");
        rgm.addResourceLocation("res/audio", "FileSystem");
        rgm.addResourceLocation("res/textures", "FileSystem");
        rgm.addResourceLocation("res/scripts", "FileSystem");
        rgm.addResourceLocation("res/gui/fonts", "FileSystem");
        rgm.addResourceLocation("res/gui/imagesets", "FileSystem");
        rgm.addResourceLocation("res/gui/looknfeel", "FileSystem");
        rgm.addResourceLocation("res/gui/schemes", "FileSystem");
        rgm.addResourceLocation("res/gui/layouts", "FileSystem");

        boost::filesystem::directory_iterator end;
        boost::filesystem::path path("res/models");
        for (boost::filesystem::directory_iterator i(path); i != end; ++i)
        {
            if (boost::filesystem::is_directory(i->status()))
            {
                rgm.addResourceLocation(i->path().string(), "FileSystem");
            }
        }

        Ogre::RenderSystem * renderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
        mRoot->setRenderSystem(renderSystem);

        // Настройки графики
        Ogre::ConfigFile cf;
        cf.load(aConfigFile);
        Ogre::ConfigFile::SettingsIterator j = cf.getSettingsIterator("OpenGL Rendering Subsystem");
        while (j.hasMoreElements())
        {
            Ogre::String name = j.peekNextKey();
            Ogre::String value = j.peekNextValue();
            renderSystem->setConfigOption(name, value);
            GetLog() << name << " " << value;
            j.moveNext();
        }

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
        GetLog() << "=== Scene manager: " << mSceneMgr->getTypeName() << "===";

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
        GetLog() << "Init OgreAL";
        mSoundManager = new OgreAL::SoundManager();
    }

    {
        GetLog() << "Init localization";
        if (const char* env = getenv("LANGUAGE"))
        {
            GetLog() << "LANGUAGE " << env;
        }
        GetLog() << "locale " << setlocale(LC_ALL, "");
        GetLog() << "bindtextdomain " << bindtextdomain("steelandconcrete", "lang");
        GetLog() << "bind_textdomain_codeset " << bind_textdomain_codeset("steelandconcrete", "UTF-8");
        GetLog() << "textdomain " << textdomain("steelandconcrete");
    }

    {
        GetLog() << "Init OIS";
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
        GetLog() << "Init CEGUI";
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
    GetLog() << "App destructor";

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

void ClientApp::BuildMainGUILayout()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.destroyAllWindows();
    CEGUI::Window* myRoot = winMgr.loadWindowLayout("Main.layout", "", "", &PropertyCallback);
    CEGUI::System::getSingleton().setGUISheet(myRoot);
    GetWindow("ServerBrowser/Port")->setText("4512");
    GetWindow("ServerBrowser/Address")->setText("localhost");

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
    GetLog() << "On connect";
    assert(!mGame);

    try
    {
        CEGUI::String port = GetWindow("ServerBrowser/Port")->getText();
        CEGUI::String address = GetWindow("ServerBrowser/Address")->getText();

        tcp::resolver resolver(mIOService);
        tcp::resolver::query query(address.c_str(), port.c_str(), boost::asio::ip::resolver_query_base::numeric_service);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        SocketSharedPtr sock(new tcp::socket(mIOService));
        sock->connect(*iterator);

        NetworkPtr net(new Network(sock));
        GetLog() << "Connected";

        PayloadMsg req;
        req.set_protocolversion(ProtocolVersion);
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
        GetWindow("MessageBox/Message")->setText(e.what());
        ShowModal("MessageBox");
    }
    return true;
}

bool ClientApp::OnCreate(const CEGUI::EventArgs& args)
{
    GetLog() << "On create";
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
    GetLog() << "Window is closed";
}


void ClientApp::MainLoop()
{
    BuildMainGUILayout();

    unsigned long frameTime = 1;

    GetLog() << "*** The Start ***";
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
    GetLog() << "*** The End ***";

    // Подчищаем игру
    delete mGame;
    mGame = NULL;
}
