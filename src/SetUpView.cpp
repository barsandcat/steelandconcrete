#include <pch.h>
#include <SetUpView.h>
#include <EgoView.h>
#include <ClientLog.h>

SetUpView::SetUpView(ClientApp & aApp):
        mApp(aApp),
        mSceneMgr(NULL),
        mCamera(NULL)
{
    mSceneMgr = mApp.OgreRoot().createSceneManager(Ogre::ST_GENERIC, "SetUpView");
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(5);

    // Create one viewport, entire window
    mApp.Window().removeAllViewports();
    Ogre::Viewport* vp = mApp.Window().addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    // Create a light
    Ogre::Light* myLight = mSceneMgr->createLight("Light0");
    myLight->setType(Ogre::Light::LT_POINT);
    myLight->setPosition(0, 40, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    Ogre::SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Camera node");
    node->setPosition(Ogre::Vector3(0, 0, 500));
    node->attachObject(mCamera);

    mExitWindow = mApp.Gui().createWindow(100, 100, 200, 200, "betagui", BetaGUI::WFT_MOVE, "SetUpView", this);
    BetaGUI::ButtonMethodPtr temp = static_cast<BetaGUI::ButtonMethodPtr>(&SetUpView::OnButton);
    mExitWindow->createButton(50, 50, "Quit", temp);
    mExitWindow->createButton(50, 65, "Continue", static_cast<BetaGUI::ButtonMethodPtr>(&SetUpView::OnContinue));
    mExitWindow->createTextInput(50, 100, 30, "SomeSetting", 3);

    GetLog() << "SetUpView ready";
}


SetUpView::~SetUpView()
{
    mExitWindow->hide();
    mApp.Gui().destroyWindow(mExitWindow);
    mApp.OgreRoot().destroySceneManager(mSceneMgr);
}


bool SetUpView::mouseMoved(const OIS::MouseEvent &arg)
{
    mApp.Gui().injectMouse(static_cast<Ogre::Real>(arg.state.X.abs),
        static_cast<Ogre::Real>(arg.state.Y.abs),
        arg.state.buttonDown(OIS::MB_Left));
    return true;
}

bool SetUpView::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mApp.Gui().injectMouse(
        static_cast<Ogre::Real>(arg.state.X.abs),
        static_cast<Ogre::Real>(arg.state.Y.abs),
        arg.state.buttonDown(OIS::MB_Left));
    return true;
}
bool SetUpView::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mApp.Gui().injectMouse(
        static_cast<Ogre::Real>(arg.state.X.abs),
        static_cast<Ogre::Real>(arg.state.Y.abs),
        arg.state.buttonDown(OIS::MB_Left));
    return true;
}
bool SetUpView::keyPressed(const OIS::KeyEvent &arg)
{
    switch (arg.key)
    {
        default:
            ;
    }
    return true;
}
bool SetUpView::keyReleased(const OIS::KeyEvent &arg)
{
    switch (arg.key)
    {
        default:
            ;
    }
    return true;
}


void SetUpView::Frame(unsigned long aFrameTime)
{
}


void SetUpView::OnButton(BetaGUI::Button*, BetaGUI::FocusState)
{
    mApp.Quit();
}

void SetUpView::OnContinue(BetaGUI::Button*, BetaGUI::FocusState)
{
    if (mApp.GetGame())
    {
        mApp.SetState(new EgoView(*mApp.GetGame()));
    }
}
