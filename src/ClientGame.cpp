#include <pch.h>
#include <ClientGame.h>
#include <ClientApp.h>
#include <Unit.pb.h>
#include <Network.h>
#include <ClientLog.h>


ClientGame::ClientGame(Ogre::SceneManager& aSceneMgr, QuickGUI::GUIManager& aGUIManager, socket_t& aSocket): mSceneMgr(aSceneMgr), mGUIManager(aGUIManager), mSocket(aSocket), mGrid(NULL)
{
    mLoadingSheet.Activate(mGUIManager);

    Ogre::Root::getSingleton().renderOneFrame();

    mGrid = new ClientGeodesicGrid(aSocket);

    mLoadingSheet.SetProgress(50.0f);
    Ogre::Root::getSingleton().renderOneFrame();

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
        float progress = 50.0f + ((i + 1) * 50.0f) / unitCount.count();
        mLoadingSheet.SetProgress(progress);
        Ogre::Root::getSingleton().renderOneFrame();
    }
    GetLog() << "Recived all units";


    // Planet
    Ogre::StaticGeometry* staticPlanet = mGrid->ConstructStaticGeometry(mSceneMgr);
    assert(staticPlanet);
    //mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mApp.GetPlanet()->ConstructDebugMesh());

    // Units
    CreateUnitEntities();

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

