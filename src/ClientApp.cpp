#include <pch.h>
#include <ClientApp.h>
#include <EgoView.h>
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
        mState(NULL),
        mGame(NULL)
{
    task::initialize(task::normal_stack);
    mRoot = new Ogre::Root("", "", "Ogre.log");

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

    mSoundManager = new OgreAL::SoundManager();

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // GUI initialization
    new QuickGUI::Root();
    QuickGUI::SkinTypeManager::getSingleton().loadTypes();

    mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");

    GetLog() << "*** Initializing OIS ***";

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

    ShowDebugOverlay(true);

    mWindowEventListener = new OgreWindowCallback(*this);
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mWindowEventListener);

    QuickGUI::GUIManagerDesc d;
//    d.sceneManager = mSceneManager;
//    d.viewport = mCamera->getViewport();
    d.queueID = Ogre::RENDER_QUEUE_OVERLAY;
    mGUIManager = QuickGUI::Root::getSingletonPtr()->createGUIManager(d);



    // 2    600
    // 3   2000
    // 4  10000
    // 5  40000
    // 6 160000

    socket_t* sock = socket_t::connect("localhost:4512");

    if (sock && sock->is_ok())
    {
        mGame = new ClientGame(*this, *sock);
        GetLog() << "Connected";
        mState = new EgoView(*mGame);
        mMouse->setEventCallback(mState);
        mKeyboard->setEventCallback(mState);
    }
}

ClientApp::~ClientApp()
{
    delete mState;
    GetLog() << "App destructor";

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

Ogre::Root & ClientApp::OgreRoot()
{
    return *mRoot;
}

OgreAL::SoundManager & ClientApp::SoundManager()
{
    return *mSoundManager;
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

            mState->Frame(frameTime);

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

Ogre::RenderWindow & ClientApp::Window()
{
    return *mWindow;
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


// Override frameStarted event to process that (don't care about frameEnded)
void ClientApp::Quit()
{
    mQuit = true;
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
