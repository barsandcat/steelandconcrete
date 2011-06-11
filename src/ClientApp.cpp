#include <pch.h>
#include <ClientApp.h>

#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <Header.pb.h>
#include <Handshake.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <ProtocolVersion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <libintl.h>
#include <locale.h>

#include <RendererModules/Ogre/CEGUIOgreResourceProvider.h>
#include <RendererModules/Ogre/CEGUIOgreImageCodec.h>


char ClientApp::RU[] = "LANGUAGE=ru";
char ClientApp::EN[] = "LANGUAGE=en";
char ClientApp::UK[] = "LANGUAGE=uk";
char ClientApp::JA[] = "LANGUAGE=ja";


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

BirdCamera& ClientApp::GetCamera()
{
    assert(mBirdCamera && "ClientApp::GetCamera() \
        нельзя вызывать в конструкторе и деструкторе ClientApp!");
    return *mBirdCamera;
}

void ClientApp::Quit()
{
    mQuit = true;
}

Ogre::SceneManager* ClientApp::mSceneMgr = NULL;
OgreAL::SoundManager* ClientApp::mSoundManager = NULL;
BirdCamera* ClientApp::mBirdCamera = NULL;
bool ClientApp::mQuit = false;

ClientApp::ClientApp(const Ogre::String aConfigFile):
    mCEGUIRenderer(NULL),
    mMouse(NULL),
    mKeyboard(NULL),
    mJoy(NULL),
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
        QuickGUI::registerScriptReader();
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
        mBirdCamera = new BirdCamera(mSceneMgr, *mWindow);
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
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));

        mInputManager = OIS::InputManager::createInputSystem(pl);

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
        mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
        try
        {
            mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
        }
        catch (...)
        {
            mJoy = NULL;
        }

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
        GetLog() << "Init OgreAL";
        mSoundManager = new OgreAL::SoundManager();
        mBirdCamera->AttachListener(mSoundManager->getListener());
    }

    {
        GetLog() << "Init CEGUI";
        CEGUI::OgreRenderer& renderer = CEGUI::OgreRenderer::create(*mWindow);
        CEGUI::OgreResourceProvider* rp = &CEGUI::OgreRenderer::createOgreResourceProvider();
        CEGUI::OgreImageCodec* ic = &CEGUI::OgreRenderer::createOgreImageCodec();
        CEGUI::System::create(renderer, rp, static_cast<CEGUI::XMLParser*>(0), ic);
        // create (load) the TaharezLook scheme file
        // (this auto-loads the TaharezLook looknfeel and imageset files)
        CEGUI::SchemeManager::getSingleton().create( "TaharezLook.scheme" );

        // create (load) a font.
        // The first font loaded automatically becomes the default font, but note
        // that the scheme might have already loaded a font, so there may already
        // be a default set - if we want the "Commonweath-10" font to definitely
        // be the default, we should set the default explicitly afterwards.
        CEGUI::FontManager::getSingleton().create( "DejaVuSans-10.font" );

        CEGUI::Window* myRoot = CEGUI::WindowManager::getSingleton().loadWindowLayout( "Main.layout" );
        CEGUI::System::getSingleton().setGUISheet( myRoot );
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

    delete mBirdCamera;
    mBirdCamera = NULL;

    mSoundManager->destroyAllSounds();
    delete mSoundManager;
    mSoundManager = NULL;

    mRoot->destroySceneManager(mSceneMgr);
    mSceneMgr = NULL;

    mInputManager->destroyInputObject(mMouse);
    mInputManager->destroyInputObject(mKeyboard);
    mInputManager->destroyInputObject(mJoy);

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

void ClientApp::OnClick(const QuickGUI::EventArgs& args)
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
}

void ClientApp::OnBrowse(const QuickGUI::EventArgs& args)
{
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

void ClientApp::OnRussian(const QuickGUI::EventArgs& args)
{
    putenv(RU);
    TriggerMsgCatalogReload();
}

void ClientApp::OnEnglish(const QuickGUI::EventArgs& args)
{
    putenv(EN);
    TriggerMsgCatalogReload();
}

void ClientApp::OnUkranian(const QuickGUI::EventArgs& args)
{
    putenv(UK);
    TriggerMsgCatalogReload();
}

void ClientApp::OnJapanese(const QuickGUI::EventArgs& args)
{
    putenv(JA);
    TriggerMsgCatalogReload();
}

void ClientApp::OnMainMenu(const QuickGUI::EventArgs& args)
{
}

void ClientApp::OnConnect(const QuickGUI::EventArgs& args)
{
    GetLog() << "On connect";
    if (!mGame)
    {
        tcp::resolver resolver(mIOService);
        tcp::resolver::query query(tcp::v4(), "localhost", "4512");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        SocketSharedPtr sock(new tcp::socket(mIOService));
        boost::system::error_code ec;
        sock->connect(*iterator, ec);

        if (!ec)
        {
            Network* net = new Network(sock);
            GetLog() << "Connected";
            ConnectionRequestMsg req;
            req.set_protocolversion(ProtocolVersion);
            net->WriteMessage(req);

            ConnectionResponseMsg res;
            net->ReadMessage(res);
            if (res.result() == CONNECTION_ALLOWED)
            {
                mGame = new ClientGame(net, res.avatar(), res.size());
            }
            else
            {
                delete net;
            }
        }
    }
}

void ClientApp::OnCreate(const QuickGUI::EventArgs& args)
{
    GetLog() << "On create";
    if (!mGame)
    {
        LaunchServer();
    }
}

bool ClientApp::keyPressed(const OIS::KeyEvent &arg)
{
    // do event injection
    CEGUI::System& cegui = CEGUI::System::getSingleton();
    bool processed = cegui.injectKeyDown(arg.key);
    processed |= cegui.injectChar(arg.text);

    if (!processed)
    {
        switch (arg.key)
        {
        case OIS::KC_W:
            mBirdCamera->Up();
            break;
        case OIS::KC_S:
            mBirdCamera->Down();
            break;
        case OIS::KC_A:
            mBirdCamera->Left();
            break;
        case OIS::KC_D:
            mBirdCamera->Right();
            break;
        case OIS::KC_SUBTRACT:
            mBirdCamera->ZoomOut();
            break;
        case OIS::KC_ADD:
            mBirdCamera->ZoomIn();
            break;
        case OIS::KC_ESCAPE:
            if (mGame)
            {
                mGame->OnEscape();
            }
        default:
            ;

        }
    }

    return true;
}

bool ClientApp::keyReleased(const OIS::KeyEvent &arg)
{
    if (CEGUI::System::getSingleton().injectKeyUp(arg.key))
    {
        return true;
    }

    switch (arg.key)
    {
    case OIS::KC_W:
        mBirdCamera->Down();
        break;
    case OIS::KC_S:
        mBirdCamera->Up();
        break;
    case OIS::KC_A:
        mBirdCamera->Right();
        break;
    case OIS::KC_D:
        mBirdCamera->Left();
        break;
    case OIS::KC_SUBTRACT:
        mBirdCamera->ZoomIn();
        break;
    case OIS::KC_ADD:
        mBirdCamera->ZoomOut();
        break;
    default:
        ;
    }

    return true;
}

bool ClientApp::mouseMoved(const OIS::MouseEvent &arg)
{
    CEGUI::System& cegui = CEGUI::System::getSingleton();

    cegui.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
    cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

    return true;
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
        switch (id)
        {
        case OIS::MB_Left:
            break;
        case OIS::MB_Right:
            mGame->OnAct();
            break;
        default:
            ;
        }
    }

    return true;
}

bool ClientApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonUp(convertOISButtonToCegui(id));
    return true;
}

//Adjust mouse clipping area
void ClientApp::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

    CEGUI::System* const sys = CEGUI::System::getSingletonPtr();
    if (sys)
        sys->notifyDisplaySizeChanged(
            CEGUI::Size(static_cast<float>(rw->getWidth()),
                        static_cast<float>(rw->getHeight())));
}

//Unattach OIS before window shutdown (very important under Linux)
void ClientApp::windowClosed(Ogre::RenderWindow* rw)
{
    mQuit = true;
    GetLog() << "Window is closed";
}


void ClientApp::MainLoop()
{
    unsigned long frameTime = 1;

    GetLog() << "*** The Start ***";
    while (!mQuit)
    {
        OgreProfile("Ogre Main Loop");
        unsigned long frameStart = mRoot->getTimer()->getMicroseconds();

        {
            OgreProfile("Update");
            Ogre::WindowEventUtilities::messagePump();
            CEGUI::System::getSingleton().injectTimePulse(frameTime / 1000000);

            mKeyboard->capture();
            mMouse->capture();
            if (mJoy)
                mJoy->capture();

            if (mGame)
            {
                mBirdCamera->UpdatePosition(frameTime);
                Ogre::Ray ray = mBirdCamera->MouseToRay(mMouse->getMouseState());
                mGame->UpdateTileUnderCursor(ray);
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
