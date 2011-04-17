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
#include <ClientTile.h>

ClientGame::ClientGame(Network* aNetwork, UnitId aAvatarId):
    mTileUnderCursor(NULL),
    mAvatar(NULL),
    mTime(0),
    mSyncTimer(1000),
    mNetwork(aNetwork)
{
    mLoadingSheet.Activate();
    GeodesicGridSizeMsg gridInfo;
    aNetwork->ReadMessage(gridInfo);
    GetLog() << "Grid info recived" << gridInfo.ShortDebugString();

    ClientGeodesicGrid grid(mTiles, gridInfo.size());

    UnitCountMsg unitCount;
    mNetwork->ReadMessage(unitCount);
    GetLog() << "Recived unit count " << unitCount.ShortDebugString();
    mTime = unitCount.time();

    for (size_t i = 0; i < unitCount.count(); ++i)
    {
        UnitMsg unit;
        mNetwork->ReadMessage(unit);
        ClientUnit* clientUnit = new ClientUnit(unit);
        mUnits.insert(std::make_pair(unit.tag(), clientUnit));

        if (unit.tag() == aAvatarId)
        {
            mAvatar = clientUnit;
            ClientGridNode& gridNode = *mTiles.at(unit.tile());
            gridNode.CreateTile(true);
            clientUnit->SetTile(gridNode.GetTile());
        }
    }
    assert(mAvatar);
    mLoadingSheet.SetProgress(90);
    GetLog() << "Recived all units";

    Ogre::Vector3 avatarPosition = mAvatar->GetTile()->GetGridNode().GetPosition();
    ClientApp::GetCamera().Goto(avatarPosition);
    ClientApp::GetCamera().SetDistance(avatarPosition.length() + 50.0f);

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

    mTileUnderCursor = mTiles.at(0);
    mSelectionMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->setScale(Ogre::Vector3(0.01));
    mSelectionMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Marker", Ogre::SceneManager::PT_SPHERE));

    mTargetMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mTargetMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Target", "TargetMarker.mesh"));
    mTargetMarker->setVisible(false);

    QuickGUI::EventHandlerManager::getSingleton().registerEventHandler("OnExit", &ClientGame::OnExit, this);

    mIngameSheet.SetTime(mTime);
    mIngameSheet.Activate();
}

ClientGame::~ClientGame()
{
    std::map< UnitId, ClientUnit* >::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
        delete i->second;
    mUnits.clear();
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        delete mTiles[i];
        mTiles[i] = NULL;
    }

    ClientApp::GetSceneMgr().clearScene();
    delete mNetwork;
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
        if (mSelectionMarker->getParent())
        {
            mSelectionMarker->getParent()->removeChild(mSelectionMarker);
        }

        if (mTileUnderCursor->GetTile())
        {
            mTileUnderCursor->GetTile()->GetNode().addChild(mSelectionMarker);
        }

    }
    mSelectionMarker->setVisible(res.first);
}

void ClientGame::OnAct()
{
    assert(mTileUnderCursor && "Тайл под курсором должен быть!");
    mAvatar->SetTarget(mTileUnderCursor->GetTile());
    mTargetMarker->getParent()->removeChild(mTargetMarker);
    mTileUnderCursor->GetTile()->GetNode().addChild(mTargetMarker);
    mTargetMarker->setVisible(true);
}

void ClientGame::OnExit(const QuickGUI::EventArgs& args)
{
    ClientApp::Quit();
    GetLog() << "OnExit";
}

void ClientGame::OnEscape()
{
    if (mSystemMenuSheet.IsActive())
    {
        mIngameSheet.Activate();
    }
    else
    {
        mSystemMenuSheet.Activate();
    }

}

ClientUnit& ClientGame::GetUnit(UnitId aUnitId)
{
    ClientUnits::iterator i = mUnits.find(aUnitId);
    if(mUnits.end() != i)
    {
        return *(i->second);
    }
    else
    {
        boost::throw_exception(std::out_of_range("No such unit " + Ogre::StringConverter::toString(aUnitId)));
    }
}

void ClientGame::LoadEvents(const ResponseMsg& changes)
{
    for (int i = 0; i < changes.changes_size(); ++i)
    {
        const ChangeMsg& change = changes.changes(i);
        if (change.has_unitmove())
        {
            const UnitMoveMsg& move = change.unitmove();
            GetUnit(move.unitid()).SetTile(mTiles.at(move.position())->GetTile());
        }
        else if (change.has_commanddone())
        {
            const CommandDoneMsg& command = change.commanddone();
            GetUnit(command.unitid()).SetTarget(NULL);
        }
        else if (change.has_remove())
        {
            const RemoveMsg& command = change.remove();
            ClientUnits::iterator i = mUnits.find(command.unitid());
            if (mUnits.end() == i)
            {
                throw std::out_of_range("Server requested removal of non existing unit " + command.ShortDebugString());
            }
            delete i->second;
            mUnits.erase(i);
        }
        else if (change.has_showtile())
        {
            TileId tileId = change.showtile().tileid();
            ClientGridNode* node = mTiles.at(tileId);
            node->CreateTile(true);
        }
    }
}

void ClientGame::Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats)
{
    mIngameSheet.UpdateStats(aStats);

    if (mSyncTimer.IsTime())
    {
        RequestMsg req;
        req.set_type(REQUEST_GET_TIME);
        req.set_time(mTime);
        req.set_last(true);

        if (mAvatar->GetTarget())
        {
            CommandMsg* command = req.add_commands();
            CommandMoveMsg* move = command->mutable_commandmove();
            move->set_unitid(mAvatar->GetUnitId());
            move->set_position(mAvatar->GetTarget()->GetGridNode().GetTileId());
        }

        mNetwork->WriteMessage(req);

        mSyncTimer.Reset(ReadResponseMessage());
    }
}

int32 ClientGame::ReadResponseMessage()
{
    ResponseMsg rsp;
    mNetwork->ReadMessage(rsp);

    int32 nextUpdate = 1000;

    switch (rsp.type())
    {
    case RESPONSE_OK:
        mTime = rsp.time();
        mIngameSheet.SetTime(mTime);
        nextUpdate = rsp.update_length();
        break;
    case RESPONSE_PART:
        LoadEvents(rsp);
        nextUpdate = ReadResponseMessage();
        break;
    case RESPONSE_NOK:
    default:
        GetLog() << rsp.ShortDebugString();
        break;
    }
    return nextUpdate;
}

