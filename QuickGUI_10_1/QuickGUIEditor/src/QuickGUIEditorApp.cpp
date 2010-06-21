#include "QuickGUIEditorApp.h"

#include "OgreRenderSystem.h"
#include "OgreRoot.h"

#include "MainForm.h"
#include <OgreGLPlugin.h>

namespace QuickGUIEditor
{
QuickGUIEditorApp::QuickGUIEditorApp() :
        mQuit(false)
{
    mOgreRoot = new Ogre::Root("", "");

    // Gl renedr system
    mOgreRoot->installPlugin(new Ogre::GLPlugin());

    mRenderSystem = mOgreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    mOgreRoot->setRenderSystem(mRenderSystem);

    // OpenGL Parameters

    //mRenderSystem->setConfigOption( "Colour Depth", "32" );
    mRenderSystem->setConfigOption("Display Frequency", "N/A");
    mRenderSystem->setConfigOption("FSAA", "0");
    mRenderSystem->setConfigOption("Full Screen", "No");
    mRenderSystem->setConfigOption("RTT Preferred Mode", "FBO");
    mRenderSystem->setConfigOption("VSync", "No");
    mRenderSystem->setConfigOption("Video Mode", "1024 x 768");

    mRenderSystem->validateConfigOptions();

    QuickGUI::registerScriptReader();
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("resources", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("user\\sheets", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("user\\skins", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("resources\\qgui.textures", "FileSystem");
#else
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../user/sheets", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../user/skins", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../resources/qgui.textures", "FileSystem");
#endif

    Ogre::RenderWindow* w = mOgreRoot->initialise(true);

    mMainForm = new MainForm(this, mOgreRoot->getAutoCreatedWindow());

    _start();
}

QuickGUIEditorApp::~QuickGUIEditorApp()
{
    delete mMainForm;

    delete mOgreRoot;
}

void QuickGUIEditorApp::_start()
{
    Ogre::Timer timer;

    std::vector<Window*>::iterator it;

    while (!mQuit)
    {
        Ogre::WindowEventUtilities::messagePump();

        mOgreRoot->renderOneFrame();

        mMainForm->update(timer.getMicroseconds() / 1000.0);
        timer.reset();
    }
}

void QuickGUIEditorApp::exit()
{
    mQuit = true;
}

bool QuickGUIEditorApp::shuttingDown()
{
    return mQuit;
}
}
