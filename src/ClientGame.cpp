#include <pch.h>
#include <ClientGame.h>
#include <ClientApp.h>
#include <Unit.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <Request.pb.h>
#include <Response.pb.h>
#include <ChangeList.pb.h>
#include <ClientApp.h>

ClientGame::ClientGame(socket_t& aSocket):
        mSocket(aSocket),
        mGrid(NULL),
        mTileUnderCursor(NULL),
        mSelectedUnit(NULL),
        mViewPortWidget(
            mIngameSheet.GetSelectedWidth(),
            mIngameSheet.GetSelectedHeight(),
            "RttTexture"
            ),
        mTime(0),
        mTurnDone(false)
{
    mIngameSheet.SetSelectedName(mViewPortWidget.GetName());
    mLoadingSheet.Activate();
    mGrid = new ClientGeodesicGrid(aSocket, mLoadingSheet);

    UnitCountMsg unitCount;
    ReadMessage(aSocket, unitCount);
    GetLog() << "Recived unit count " << unitCount.ShortDebugString();
    mTime = unitCount.time();

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
    Ogre::StaticGeometry* staticPlanet = mGrid->ConstructStaticGeometry();
    mLoadingSheet.SetProgress(90);
    assert(staticPlanet);
    //mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mApp.GetPlanet()->ConstructDebugMesh());

    // Units
    CreateUnitEntities();
    mLoadingSheet.SetProgress(100);

    // Create a light
    Ogre::Light* myLight = ClientApp::GetSceneMgr().createLight("Light0");
    myLight->setType(Ogre::Light::LT_DIRECTIONAL);
    myLight->setPosition(50, 0, 0);
    myLight->setDirection(-1, 0, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    mTileUnderCursor = &mGrid->GetTile(0);
    mSelectionMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->setScale(Ogre::Vector3(0.01));
    mSelectionMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Marker", Ogre::SceneManager::PT_SPHERE));

    mTargetMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mTargetMarker->setScale(Ogre::Vector3(0.01));
    mTargetMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Target", Ogre::SceneManager::PT_SPHERE));
    mTargetMarker->setVisible(false);

    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnExit", &ClientGame::OnExit, this);
    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnTurn", &ClientGame::OnTurn, this);

    mIngameSheet.SetTime(mTime);
    mIngameSheet.Activate();
}

ClientGame::~ClientGame()
{
    delete mGrid;

    std::map< UnitId, ClientUnit* >::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
      delete i->second;
    mUnits.clear();

    ClientApp::GetSceneMgr().clearScene();
    mSocket.close();
    delete &mSocket;
}

void ClientGame::CreateUnitEntities() const
{
    std::map< int, ClientUnit* >::const_iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        i->second->CreateEntity();
    }
}

void ClientGame::UpdateTileUnderCursor(Ogre::Ray& aRay)
{
    Ogre::Sphere sphere(Ogre::Vector3::ZERO, 1.0f);
    std::pair<bool, Ogre::Real> res = aRay.intersects(sphere);
    if (res.first)
    {
        Ogre::Vector3 position(aRay.getPoint(res.second));
        mTileUnderCursor = mTileUnderCursor->GetTileAtPosition(position);
        mSelectionMarker->getParent()->removeChild(mSelectionMarker);
        mTileUnderCursor->GetNode().addChild(mSelectionMarker);
    }
    mSelectionMarker->setVisible(res.first);
}

void ClientGame::Select()
{
    assert(mTileUnderCursor && "Тайл под курсором должен быть!");
    mSelectedUnit = mTileUnderCursor->GetUnit();
    if (mSelectedUnit)
    {
        ClientTile* tile = mSelectedUnit->GetTarget();
        if (tile)
        {
            mTargetMarker->getParent()->removeChild(mTargetMarker);
            tile->GetNode().addChild(mTargetMarker);
            mTargetMarker->setVisible(true);
        }
        else
        {
            mTargetMarker->setVisible(false);
        }
    }
    else
    {
        mTargetMarker->setVisible(false);
    }
}

void ClientGame::Act()
{
    assert(mTileUnderCursor && "Тайл под курсором должен быть!");
    if (mSelectedUnit)
    {
        mSelectedUnit->SetTarget(mTileUnderCursor);
        mTargetMarker->getParent()->removeChild(mTargetMarker);
        mTileUnderCursor->GetNode().addChild(mTargetMarker);
        mTargetMarker->setVisible(true);
    }
}

void ClientGame::OnExit(const QuickGUI::EventArgs& args)
{
    ClientApp::Quit();
    GetLog() << "OnExit";
}
void ClientGame::OnTurn(const QuickGUI::EventArgs& args)
{
    if (!mTurnDone)
    {
        RequestMsg req;
        req.set_type(ConfirmTime);
        req.set_time(mTime);
        GetLog() << req.ShortDebugString();
        WriteMessage(mSocket, req);

        ResponseMsg rsp;
        ReadMessage(mSocket, rsp);
        if (rsp.type() == Ok)
        {
            mTurnDone = true;
            GetLog() << "Turn done";
        }
        else
        {
            GetLog() << rsp.ShortDebugString();
        }

    }
}

void ClientGame::LoadEvents(const ChangeListMsg& changes)
{
    for (int i = 0; i < changes.changes_size(); ++i)
    {
        const ChangeMsg& change = changes.changes(i);
        if (change.has_unitmove())
        {
            const UnitMoveMsg& move = change.unitmove();
            mUnits[move.unitid()]->SetPosition(mGrid->GetTile(move.position()));
        }
    }
}

void ClientGame::Update(unsigned long aFrameTime)
{
    mIngameSheet.SetSelectedVisible(mSelectedUnit != NULL);
    if (mSelectedUnit)
    {
        mViewPortWidget.SetNode(&mSelectedUnit->GetNode());
    }

    if (mTurnDone)
    {
        RequestMsg req;
        req.set_type(GetTime);
        WriteMessage(mSocket, req);

        ResponseMsg rsp;
        ReadMessage(mSocket, rsp);
        switch (rsp.type())
        {
        case NewTime:
            if (rsp.has_time())
            {
                mTime = rsp.time();
                mIngameSheet.SetTime(mTime);
                mTurnDone = false;
                GetLog() << "New time " << mTime;
                ChangeListMsg changes;
                ReadMessage(mSocket, changes);
                LoadEvents(changes);
            }
            else
              GetLog() << rsp.ShortDebugString();
            break;
        case PleaseWait:
            break;
        default:
            GetLog() << rsp.ShortDebugString();
            break;
        }
    }
}

