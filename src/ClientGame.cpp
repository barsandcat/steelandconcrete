#include <pch.h>
#include <ClientGame.h>

#include <ClientApp.h>
#include <Network.h>
#include <ClientLog.h>
#include <Payload.pb.h>
#include <ChangeList.pb.h>
#include <ClientApp.h>
#include <ClientTile.h>

#include <CEGUILocalization.h>

ClientGame::ClientUnits ClientGame::mUnits;

void ClientGame::EraseUnitId(UnitId aUnitId)
{
    mUnits.erase(aUnitId);
}

ClientGame::ClientGame(NetworkPtr aNetwork, UnitId aAvatarId, int32 aGridSize):
    mTileUnderCursor(NULL),
    mAvatar(NULL),
    mTime(0),
    mSyncTimer(1000),
    mServerUpdateLength(1000),
    mNetwork(aNetwork)
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* myRoot = winMgr.loadWindowLayout("Game.layout", "", "", &PropertyCallback);
    CEGUI::System::getSingleton().setGUISheet(myRoot);

    winMgr.getWindow("InGameMenu/Exit")->
        subscribeEvent(CEGUI::PushButton::EventClicked,
                       CEGUI::Event::Subscriber(&ClientGame::OnExit, this));

    ClientGeodesicGrid grid(mTiles, aGridSize);

    LoadAvatar();

    mAvatar = GetUnit(aAvatarId);
    if (!mAvatar)
    {
        throw std::runtime_error("No avatar!");
    }

    Ogre::Vector3 avatarPosition = mAvatar->GetTile()->GetPosition();
    ClientApp::GetCamera().Goto(avatarPosition);
    ClientApp::GetCamera().SetDistance(avatarPosition.length() + 50.0f);

    // Create a light
    Ogre::Light* myLight = ClientApp::GetSceneMgr().createLight("Light0");
    myLight->setType(Ogre::Light::LT_DIRECTIONAL);
    myLight->setPosition(50, 0, 0);
    myLight->setDirection(-1, 0, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    mTileUnderCursor = mTiles.at(0);
    mSelectionMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->setScale(Ogre::Vector3(0.1));
    mSelectionMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Marker", Ogre::SceneManager::PT_SPHERE));

    mTargetMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mTargetMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Target", "TargetMarker.mesh"));
    mTargetMarker->setVisible(false);
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
}

void ClientGame::UpdateTileUnderCursor(Ogre::Ray& aRay)
{
    Ogre::Real radius = mTiles[0]->GetPosition().length();
    Ogre::Sphere sphere(Ogre::Vector3::ZERO, radius);
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
    ClientTile* tile = mTileUnderCursor->GetTile();
    if (tile)
    {
        mTargetMarker->getParent()->removeChild(mTargetMarker);
        tile->GetNode().addChild(mTargetMarker);
        mTargetMarker->setVisible(true);
        boost::shared_ptr<PayloadMsg> req(new PayloadMsg());
        CommandMoveMsg* move = req->mutable_commandmove();
        move->set_position(mTileUnderCursor->GetTileId());
        mNetwork->Request(boost::bind(&ClientGame::OnPayloadMsg, this, _1), req);
    }
}

bool ClientGame::OnExit(const CEGUI::EventArgs& args)
{
    ClientApp::Quit();
    GetLog() << "OnExit";
    return true;
}

void ClientGame::OnEscape()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.getWindow("InGameMenu")->setVisible(true);
}

ClientUnit* ClientGame::GetUnit(UnitId aUnitId)
{
    ClientUnits::iterator i = mUnits.find(aUnitId);
    if(mUnits.end() != i)
    {
        return i->second;
    }
    return NULL;
}

void ClientGame::LoadEvents(PayloadPtr aPayloadMsg)
{
    for (int i = 0; i < aPayloadMsg->changes_size(); ++i)
    {
        const ChangeMsg& change = aPayloadMsg->changes(i);
        if (change.has_unitenter())
        {
            const UnitEnterMsg& move = change.unitenter();
            ClientUnit* unit = GetUnit(move.unitid());
            if (!unit)
            {
                if (move.has_visualcode())
                {
                    ClientUnit* unit = new ClientUnit(move.unitid(), move.visualcode(), mTiles.at(move.to())->GetTile());
                    mUnits.insert(std::make_pair(move.unitid(), unit));
                }
            }
            else
            {
                unit->SetTile(mTiles.at(move.to())->GetTile());
            }
        }

        if (change.has_unitleave())
        {
            const UnitLeaveMsg& leave = change.unitleave();
            delete GetUnit(leave.unitid());
        }

        if (change.has_commanddone())
        {
            mTargetMarker->setVisible(false);
        }

        if (change.has_remove())
        {
            const RemoveMsg& command = change.remove();
            delete GetUnit(command.unitid());
        }

        if (change.has_showtile())
        {
            TileId tileId = change.showtile().tileid();
            ClientGridNode* node = mTiles.at(tileId);
            node->CreateTile(change.showtile().whater() == 0);
        }

        if (change.has_hidetile())
        {
            TileId tileId = change.hidetile().tileid();
            ClientGridNode* node = mTiles.at(tileId);
            node->DestroyTile();
        }
    }
}

void ClientGame::Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats)
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.getWindow("FPS")->setText(Ogre::StringConverter::toString(aStats.avgFPS));

    std::for_each(mUnits.begin(), mUnits.end(),
                  boost::bind(&ClientUnit::UpdateMovementAnimation,
                              boost::bind(&ClientUnits::value_type::second, _1), aFrameTime));

    if (mSyncTimer.IsTime())
    {
        boost::shared_ptr<PayloadMsg> req(new PayloadMsg());
        req->set_time(mTime);
        mNetwork->Request(boost::bind(&ClientGame::OnPayloadMsg, this, _1), req);
        mSyncTimer.Reset(mServerUpdateLength);
    }
}

void ClientGame::LoadAvatar()
{
    PayloadMsg req;
    req.set_time(0);
    req.set_last(true);
    mNetwork->WriteMessage(req);

    boost::shared_ptr< PayloadMsg > rsp;
    do
    {
        rsp.reset(new PayloadMsg());
        mNetwork->ReadMessage(*rsp);
        OnPayloadMsg(rsp);
    }
    while (!rsp->last());
}

void ClientGame::OnPayloadMsg(PayloadPtr aPayloadMsg)
{
    if (aPayloadMsg->has_time())
    {
        mTime = aPayloadMsg->time();
        CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
        winMgr.getWindow("Time")->setText(Ogre::StringConverter::toString(static_cast<long>(mTime)));
    }
    if (aPayloadMsg->has_update_length())
    {
        mServerUpdateLength = aPayloadMsg->update_length();
        mSyncTimer.Reset(mServerUpdateLength);
    }
    LoadEvents(aPayloadMsg);
    if (aPayloadMsg->has_reason())
    {
        GetLog() << aPayloadMsg->ShortDebugString();
    }
}
