#include <pch.h>
#include <ClientGame.h>
#include <EgoView.h>
#include <ClientLog.h>
#include <ClientApp.h>

EgoView::EgoView(ClientGame& aGame):
        mGame(aGame),
        mBirdCamera(NULL),
        mSelectedTile(&aGame.GetGrid().GetTile(0)),
        mSceneMgr(NULL)
{
    // Scene manager
    mSceneMgr = mGame.GetApp().OgreRoot().createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "EgoView");
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    mSceneMgr->setAmbientLight(Ogre::ColourValue::White);

    // Create the camera
    mBirdCamera = new BirdCamera(mSceneMgr, mGame.GetApp().Window());

    // Planet
    Ogre::StaticGeometry* staticPlanet = mGame.GetGrid().ConstructStaticGeometry(*mSceneMgr);
    assert(staticPlanet);
    //mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mApp.GetPlanet()->ConstructDebugMesh());

    // Units
    mGame.CreateUnitEntities(*mSceneMgr);


    // Create a light
    Ogre::Light* myLight = mSceneMgr->createLight("Light0");
    myLight->setType(Ogre::Light::LT_DIRECTIONAL);
    myLight->setPosition(50, 0, 0);
    myLight->setDirection(-1, 0, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    // Create gui
    mExitWindow = mGame.GetApp().Gui().createWindow(100, 100, 200, 200, "betagui", BetaGUI::WFT_MOVE, "EgoView", this);
    BetaGUI::ButtonMethodPtr temp = static_cast<BetaGUI::ButtonMethodPtr>(&EgoView::OnButton);
    mExitButton = mExitWindow->createButton(50, 50, "Pause", temp);

    mSelectedTile = &mGame.GetGrid().GetTile(0);
    mSelectionMarker = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->attachObject(mSceneMgr->createEntity("Marker", Ogre::SceneManager::PT_SPHERE));
    mSelectionMarker->setScale(Ogre::Vector3(0.001));

    GetLog() << "EgoView ready";
}


EgoView::~EgoView()
{
    delete mBirdCamera;
    mBirdCamera = NULL;
    //mApp.SoundManager().destroyAllSounds();
    mExitWindow->hide();
    mGame.GetApp().Gui().destroyWindow(mExitWindow);
    mGame.GetApp().OgreRoot().destroySceneManager(mSceneMgr);
}


bool EgoView::mouseMoved(const OIS::MouseEvent &arg)
{
    mGame.GetApp().Gui().injectMouse(Ogre::Real(arg.state.X.abs),
                           Ogre::Real(arg.state.Y.abs),
                           arg.state.buttonDown(OIS::MB_Left));
	return true;
}

void EgoView::UpdateSelectedTilePosition(const OIS::MouseState &aState)
{
    Ogre::Ray ray;
    Ogre::Sphere sphere(Ogre::Vector3::ZERO, 1.0f);

    mBirdCamera->MouseToRay(aState, &ray);

    std::pair<bool, Ogre::Real> res = ray.intersects(sphere);
    if (res.first)
    {
        Ogre::Vector3 position(ray.getPoint(res.second));
        mSelectedTile = mSelectedTile->GetTileAtPosition(position);
        mSelectionMarker->setPosition(mSelectedTile->GetPosition());
    }
    mSelectionMarker->setVisible(res.first);
}

bool EgoView::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mGame.GetApp().Gui().injectMouse(
        static_cast<Ogre::Real>(arg.state.X.abs),
        static_cast<Ogre::Real>(arg.state.Y.abs),
        arg.state.buttonDown(OIS::MB_Left));
    return true;
}
bool EgoView::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mGame.GetApp().Gui().injectMouse(
        static_cast<Ogre::Real>(arg.state.X.abs),
        static_cast<Ogre::Real>(arg.state.Y.abs),
        arg.state.buttonDown(OIS::MB_Left));
    return true;
}
bool EgoView::keyPressed(const OIS::KeyEvent &arg)
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
    return true;
}
bool EgoView::keyReleased(const OIS::KeyEvent &arg)
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
    return true;
}


void EgoView::Frame(unsigned long aFrameTime)
{
    // Camera movement
    mBirdCamera->UpdatePosition(aFrameTime);
    UpdateSelectedTilePosition(mGame.GetApp().GetMouse()->getMouseState());
}


void EgoView::OnButton(BetaGUI::Button*, BetaGUI::FocusState)
{
}

