#include <pch.h>
#include <ClientApp.h>

#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <Header.pb.h>
#include <Vector.pb.h>
#include <Handshake.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <ProtocolVersion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>



QuickGUI::GUIManager& ClientApp::GetGuiMgr()
{
    assert(ClientApp::mGUIManager && "ClientApp::GetGuiMgr() \
        нельзя вызывать в конструктори и деструкторе ClientApp!");
    return *ClientApp::mGUIManager;
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

QuickGUI::GUIManager* ClientApp::mGUIManager = NULL;
Ogre::SceneManager* ClientApp::mSceneMgr = NULL;
OgreAL::SoundManager* ClientApp::mSoundManager = NULL;
BirdCamera* ClientApp::mBirdCamera = NULL;
bool ClientApp::mQuit = false;

ClientApp::ClientApp(const Ogre::String aConfigFile):
    mMouse(NULL),
    mKeyboard(NULL),
    mJoy(NULL),
    mGame(NULL)
{
    // Только для инициализации!
    // Ни каких вызовов других функий этого класа, что бы небыло необходимости
    // проверять поля на то что они инициализированы.
    {
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
        rgm.addResourceLocation("res/audio", "FileSystem");
        rgm.addResourceLocation("res/textures", "FileSystem");
        rgm.addResourceLocation("res/scripts", "FileSystem");
        rgm.addResourceLocation("res/quickgui", "FileSystem");

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

        mWindowEventListener = new OgreWindowCallback(*this);
        Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mWindowEventListener);

        // Scene manager
        mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "EgoView");
        mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
        mSceneMgr->setAmbientLight(Ogre::ColourValue::White);

        // Create the camera
        mBirdCamera = new BirdCamera(mSceneMgr, *mWindow);
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
        GetLog() << "Init QuickGUI";

        new QuickGUI::Root();
        QuickGUI::SkinTypeManager::getSingleton().loadTypes();

        QuickGUI::GUIManagerDesc d;
        d.sceneManager = mSceneMgr;
        d.viewport = mBirdCamera->GetViewPort();
        d.queueID = Ogre::RENDER_QUEUE_OVERLAY;
        mGUIManager = QuickGUI::Root::getSingletonPtr()->createGUIManager(d);
    }

    {
        GetLog() << "Init app UI";
        mMainMenu = new MainMenuSheet();
        mServerBrowserSheet = new ServerBrowserSheet();
        mMainMenu->Activate(*mGUIManager);
        mGUIManager->notifyViewportDimensionsChanged();
        QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnBrowse", &ClientApp::OnBrowse, this);
        QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnConnect", &ClientApp::OnConnect, this);
        QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnCreate", &ClientApp::OnCreate, this);
        QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnMainMenu", &ClientApp::OnMainMenu, this);
        QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnClick", &ClientApp::OnClick, this);

    }
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
    mServerBrowserSheet->Activate(*mGUIManager);
}

void ClientApp::OnMainMenu(const QuickGUI::EventArgs& args)
{
    mMainMenu->Activate(*mGUIManager);
}

void ClientApp::OnConnect(const QuickGUI::EventArgs& args)
{
    GetLog() << "On connect";
    if (!mGame)
    {
        tcp::resolver resolver(mIOService);
        tcp::resolver::query query(tcp::v4(), mServerBrowserSheet->GetAddress(), mServerBrowserSheet->GetPort());
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
                mGame = new ClientGame(net, res.avatar());
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

ClientApp::~ClientApp()
{
    // Зачистка системных библиотек.
    // ТОЛЬКО ДЛЯ ТОГО ЧТО БЫЛО ИНИЦИАЛИЗОРВАНО В КОНСТРУКТОЕ
    // все остальное должно быть уже почищено!
    GetLog() << "App destructor";

    delete mMainMenu;
    mMainMenu = NULL;

    delete mBirdCamera;
    mBirdCamera = NULL;

    mSoundManager->destroyAllSounds();
    delete mSoundManager;
    mSoundManager = NULL;

    mRoot->destroySceneManager(mSceneMgr);
    mSceneMgr = NULL;

    DestroyOIS(mWindow);

    delete QuickGUI::Root::getSingletonPtr();

    delete mRoot;
    mRoot = NULL;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mWindowEventListener);
    delete mWindowEventListener;
    mWindowEventListener = NULL;

    delete mOctreePlugin;
    mOctreePlugin = NULL;

    delete mGLPlugin;
    mGLPlugin = NULL;
}

void ClientApp::MainLoop()
{
    unsigned long frameTime = 1;

    GetLog() << "*** The Start ***";
    while (!mQuit)
    {
        OgreProfileBegin("Ogre Main Loop");
        unsigned long frameStart = mRoot->getTimer()->getMicroseconds();

        Ogre::WindowEventUtilities::messagePump();

        if (!mWindow->isClosed())
        {
            mKeyboard->capture();
            mMouse->capture();
            if (mJoy)
                mJoy->capture();

            Frame(frameTime);

            mRoot->renderOneFrame();
        }
        else
        {
            mQuit = true;
            GetLog() << "Window is closed";
        }

        frameTime = mRoot->getTimer()->getMicroseconds() - frameStart;
        OgreProfileEnd("Ogre Main Loop");
    }
    GetLog() << "*** The End ***";

    // Подчищаем игру
    delete mGame;
    mGame = NULL;
}

void ClientApp::UpdateOISMouseClipping(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void ClientApp::UpdateSheetSize(Ogre::RenderWindow* rw)
{
    mGUIManager->notifyViewportDimensionsChanged();
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
    QuickGUI::Sheet* sheet = mGUIManager->getActiveSheet();
    if (sheet)
    {
        sheet->setDimensions(QuickGUI::Rect(0, 0, width, height));
    }
}


//Unattach OIS before window shutdown (very important under Linux)
void ClientApp::DestroyOIS(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if (rw == mWindow)
    {
        if (mInputManager)
        {
            mInputManager->destroyInputObject(mMouse);
            mInputManager->destroyInputObject(mKeyboard);
            mInputManager->destroyInputObject(mJoy);

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
            GetLog() << "OIS destroyed";
        }
    }
}

bool ClientApp::keyPressed(const OIS::KeyEvent &arg)
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
    default:
        ;

    }

    mGUIManager->injectChar(static_cast<Ogre::UTFString::unicode_char>(arg.text));
    mGUIManager->injectKeyDown(static_cast<QuickGUI::KeyCode>(arg.key));

    return true;
}

bool ClientApp::keyReleased(const OIS::KeyEvent &arg)
{
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

    mGUIManager->injectKeyUp(static_cast<QuickGUI::KeyCode>(arg.key));

    return true;
}

bool ClientApp::mouseMoved(const OIS::MouseEvent &arg)
{
    mGUIManager->injectMousePosition(arg.state.X.abs, arg.state.Y.abs);

    float z = arg.state.Z.rel;
    if (z != 0)
        mGUIManager->injectMouseWheelChange(z);

    return true;
}

bool ClientApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mGUIManager->injectMouseButtonDown(static_cast<QuickGUI::MouseButtonID>(id));
    if (mGame)
    {
        switch (id)
        {
        case OIS::MB_Left:
            break;
        case OIS::MB_Right:
            mGame->Act();
            break;
        default:
            ;
        }
    }

    return true;
}

bool ClientApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mGUIManager->injectMouseButtonUp(static_cast<QuickGUI::MouseButtonID>(id));

    return true;
}

void ClientApp::Frame(unsigned long aFrameTime)
{
    // Camera movement
    mBirdCamera->UpdatePosition(aFrameTime);
    if (mGame)
    {
        Ogre::Ray ray = mBirdCamera->MouseToRay(mMouse->getMouseState());
        mGame->UpdateTileUnderCursor(ray);
        mGame->Update(aFrameTime, mWindow->getStatistics());
    }
}
