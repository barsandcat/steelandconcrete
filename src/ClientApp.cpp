#include <pch.h>
#include <ClientApp.h>
#include <OgreOctreePlugin.h>
#include <OgreGLPlugin.h>
#include <sockio.h>
#include <task.h>
#include <Header.pb.h>
#include <Vector.pb.h>
#include <Network.h>
#include <ClientLog.h>

ClientApp::ClientApp(const Ogre::String aConfigFile):
        mQuit(false),
        mMouse(NULL),
        mKeyboard(NULL),
        mJoy(NULL),
        mGame(NULL),
        mBirdCamera(NULL),
        mSceneMgr(NULL)
{
    task::initialize(task::normal_stack);

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
        QuickGUI::registerScriptReader();
        Ogre::ConfigFile cf;
        cf.load(aConfigFile);
        Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator("Resources");
        while (i.hasMoreElements())
        {
            Ogre::String name = i.peekNextKey();
            Ogre::String value = i.peekNextValue();
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(value, name);
            i.moveNext();
        }


        Ogre::RenderSystem * renderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
        mRoot->setRenderSystem(renderSystem);

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
        UpdateOISMouseClipping(mWindow);

        mMouse->setEventCallback(this);
        mKeyboard->setEventCallback(this);
    }

    {
        GetLog() << "Init OgreAL";
        mSoundManager = new OgreAL::SoundManager();
    }

    {
        GetLog() << "Init QuickGUI";

        mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
        ShowDebugOverlay(true);

        new QuickGUI::Root();
        QuickGUI::SkinTypeManager::getSingleton().loadTypes();

        QuickGUI::GUIManagerDesc d;
        d.sceneManager = mSceneMgr;
        d.viewport = mBirdCamera->GetViewPort();
        d.queueID = Ogre::RENDER_QUEUE_OVERLAY;
        mGUIManager = QuickGUI::Root::getSingletonPtr()->createGUIManager(d);
    }

    {
        mMainMenu = new MainMenuSheet();
        mMainMenu->Activate(*mGUIManager);
        mGUIManager->notifyViewportDimensionsChanged();
    }
    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnConnect", &ClientApp::OnConnect, this);
}

void ClientApp::OnConnect(const QuickGUI::EventArgs& args)
{
    GetLog() << "On connect";
    if (!mGame)
    {
        socket_t* sock = socket_t::connect("localhost:4512");
        if (sock && sock->is_ok())
        {
            GetLog() << "Connected";
            mGame = new ClientGame(*mSceneMgr, *mGUIManager, *sock);
        }
    }
}

ClientApp::~ClientApp()
{
    GetLog() << "App destructor";

    delete mMainMenu;

    delete mBirdCamera;
    mBirdCamera = NULL;
    mSoundManager->destroyAllSounds();
    mRoot->destroySceneManager(mSceneMgr);


    delete mSoundManager;
    mSoundManager = NULL;

    DestroyOIS(mWindow);

    delete mGame;
    mGame = NULL;

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

            UpdateStats();
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
        sheet->setDimensions(QuickGUI::Rect(0,0, width, height));
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

void ClientApp::ShowDebugOverlay(bool show)
{
    if (mDebugOverlay)
    {
        if (show)
            mDebugOverlay->show();
        else
            mDebugOverlay->hide();
    }
}

void ClientApp::UpdateStats()
{
    static Ogre::String currFps = "Current FPS: ";
    static Ogre::String avgFps = "Average FPS: ";
    static Ogre::String bestFps = "Best FPS: ";
    static Ogre::String worstFps = "Worst FPS: ";
    static Ogre::String tris = "Triangle Count: ";
    static Ogre::String batches = "Batch Count: ";

    // update stats when necessary
    try
    {
        Ogre::OverlayElement* guiAvg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
        Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
        Ogre::OverlayElement* guiBest = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
        Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

        const Ogre::RenderTarget::FrameStats& stats = mWindow->getStatistics();
        guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
        guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
        guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS)
                            + " " + Ogre::StringConverter::toString(stats.bestFrameTime) + " ms");
        guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(stats.worstFPS)
                             + " " + Ogre::StringConverter::toString(stats.worstFrameTime) + " ms");

        Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
        guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

        Ogre::OverlayElement* guiBatches = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
        guiBatches->setCaption(batches + Ogre::StringConverter::toString(stats.batchCount));
    }
    catch (...)
    {
        /* ignore */
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
        mGame->UpdateSelectedTilePosition(ray);
        mQuit = mGame->IsQuit();
    }
}
