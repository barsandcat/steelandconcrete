#include <pch.h>
#include <ClientGame.h>
#include <ClientApp.h>
#include <Unit.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <Request.pb.h>


ClientGame::ClientGame(Ogre::SceneManager& aSceneMgr, QuickGUI::GUIManager& aGUIManager, socket_t& aSocket):
    mSceneMgr(aSceneMgr),
    mGUIManager(aGUIManager),
    mSocket(aSocket),
    mGrid(NULL),
    mQuit(false)
{
    mLoadingSheet.Activate(mGUIManager);
    mGrid = new ClientGeodesicGrid(aSocket, mLoadingSheet);

    UnitCountMsg unitCount;
    ReadMessage(aSocket, unitCount);
    GetLog() << "Recived unit count " << unitCount.ShortDebugString();

    for (size_t i = 0; i < unitCount.count(); ++i)
    {
        UnitMsg unit;
        ReadMessage(aSocket, unit);
        mUnits.insert(std::make_pair(
                          unit.tag(),
                          new ClientUnit(mGrid->GetTile(unit.tile()), unit.tag())
                      ));
    }
    mLoadingSheet.SetProgress(60);
    GetLog() << "Recived all units";

    // Planet
    Ogre::StaticGeometry* staticPlanet = mGrid->ConstructStaticGeometry(mSceneMgr);
    mLoadingSheet.SetProgress(90);
    assert(staticPlanet);
    //mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mApp.GetPlanet()->ConstructDebugMesh());

    // Units
    CreateUnitEntities();
    mLoadingSheet.SetProgress(100);

    // Create a light
    Ogre::Light* myLight = mSceneMgr.createLight("Light0");
    myLight->setType(Ogre::Light::LT_DIRECTIONAL);
    myLight->setPosition(50, 0, 0);
    myLight->setDirection(-1, 0, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    mSelectedTile = &mGrid->GetTile(0);
    mSelectionMarker = mSceneMgr.getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->attachObject(mSceneMgr.createEntity("Marker", Ogre::SceneManager::PT_SPHERE));
    mSelectionMarker->setScale(Ogre::Vector3(0.001));

    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnExit", &ClientGame::OnExit, this);
    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnTurn", &ClientGame::OnTurn, this);

    mIngameSheet.Activate(mGUIManager);
}

ClientGame::~ClientGame()
{
    mSocket.close();
    delete &mSocket;
}

void ClientGame::CreateUnitEntities() const
{
    std::map< int, ClientUnit* >::const_iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        i->second->CreateEntity(mSceneMgr);
    }
}

void ClientGame::UpdateSelectedTilePosition(Ogre::Ray& aRay)
{
    Ogre::Sphere sphere(Ogre::Vector3::ZERO, 1.0f);
    std::pair<bool, Ogre::Real> res = aRay.intersects(sphere);
    if (res.first)
    {
        Ogre::Vector3 position(aRay.getPoint(res.second));
        mSelectedTile = mSelectedTile->GetTileAtPosition(position);
        mSelectionMarker->setPosition(mSelectedTile->GetPosition());
    }
    mSelectionMarker->setVisible(res.first);
}

void ClientGame::OnExit(const QuickGUI::EventArgs& args)
{
    mQuit = true;
    GetLog() << "OnExit";
}
void ClientGame::OnTurn(const QuickGUI::EventArgs& args)
{
    RequestMsg msg;
    msg.set_type(Ready);
    WriteMessage(mSocket, msg);
    GetLog() << "OnTurn";
}

