#include <pch.h>
#include <ClientGame.h>

#include <ClientApp.h>
#include <ServerProxy.h>
#include <Payload.pb.h>
#include <ChangeList.pb.h>
#include <ClientApp.h>
#include <ClientTile.h>

#include <CEGUILocalization.h>
#include <GUI.h>

ClientGame::ClientUnits ClientGame::mUnits;

ClientGame::ClientGame(ServerProxyPtr aServerProxy, UnitId aAvatar, int32 aGridSize):
    mTileUnderCursor(NULL),
    mTime(0),
    mSyncTimer(1000),
    mServerUpdateLength(1000),
    mServerProxy(aServerProxy),
    mLifeTime(0),
    mAvatar(aAvatar),
    mFreeCamera(false)
{
    ClientGeodesicGrid grid(mTiles, aGridSize);

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

    mAxes = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mAxes->attachObject(ClientApp::GetSceneMgr().createEntity("axes", "axes.mesh"));

    GetWindow("StatusPanel/User")->setText(GetWindow("ServerBrowser/Login")->getText());
    GetWindow("StatusPanel/Server")->setText(GetWindow("ServerBrowser/Address")->getText());

    Hide("ServerBrowser");
    Show("StatusPanel");

    RequestUpdate();
}

ClientGame::~ClientGame()
{
    std::map< UnitId, ClientUnit* >::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        delete i->second;
    }
    mUnits.clear();

    GeodesicGrid<ClientGridNode>::Tiles::iterator j = mTiles.begin();
    for (; j != mTiles.end(); ++j)
    {
        delete *j;
    }
    mTiles.clear();

    Hide("InGameMenu");
    Hide("StatusPanel");
    Show("Main/Menu");

    ClientApp::GetSceneMgr().clearScene();
}

void ClientGame::UpdateTileUnderCursor(Ogre::Ray aRay)
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

void ClientGame::mouseMoved(const OIS::MouseEvent& arg)
{


}

void ClientGame::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    switch (id)
    {
    case OIS::MB_Left:
        OnAct();
        break;
    case OIS::MB_Right:
        break;
    default:
        ;
    }

}
void ClientGame::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{

}
void ClientGame::keyPressed(const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
    case OIS::KC_ESCAPE:
    {
        CEGUI::EventArgs args;
        OnEscape(args);
        break;
    }
    case OIS::KC_F3:
    {
        mFreeCamera = !mFreeCamera;
        ClientApp::GetCamera().setFixedYawAxis(mFreeCamera, ClientApp::GetCamera().getPosition().normalisedCopy());
        break;
    }

    default:
        ;
    }
}
void ClientGame::keyReleased(const OIS::KeyEvent& arg)
{

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

        PayloadPtr req(new PayloadMsg());
        CommandMoveMsg* move = req->mutable_commandmove();
        move->set_position(mTileUnderCursor->GetTileId());
        mServerProxy->Request(boost::bind(&ClientGame::OnPayloadMsg, this, _1), req);
    }
}


bool ClientGame::OnEscape(const CEGUI::EventArgs& args)
{
    CEGUI::Window* inGameMenu = GetWindow("InGameMenu");
    inGameMenu->setVisible(!inGameMenu->isVisible());
    return true;
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

void ClientGame::DeleteUnit(UnitId aUnitId)
{
    ClientUnits::iterator i = mUnits.find(aUnitId);
    if(mUnits.end() != i)
    {
        ClientUnit* unit = i->second;
        unit->GetUnitTile().RemoveUnit();
        delete unit;
        mUnits.erase(i);
    }
}

void ClientGame::CreateUnit(UnitId aUnitId, uint32 aVisualCode, TileId aTile)
{
    ClientUnit* unit = new ClientUnit(aUnitId, aVisualCode, mTiles.at(aTile));
    mUnits.insert(std::make_pair(aUnitId, unit));
}

void ClientGame::LoadEvents(ConstPayloadPtr aPayloadMsg)
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
                    CreateUnit(move.unitid(), move.visualcode(), move.to());
                }
            }
            else
            {
                unit->SetTile(mTiles.at(move.to()));
            }
        }

        if (change.has_unitleave())
        {
            const UnitLeaveMsg& leave = change.unitleave();
            DeleteUnit(leave.unitid());
        }

        if (change.has_commanddone())
        {
            mTargetMarker->setVisible(false);
        }

        if (change.has_remove())
        {
            const RemoveMsg& command = change.remove();
            DeleteUnit(command.unitid());
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
            if (node->GetUnit())
            {
                DeleteUnit(node->GetUnit()->GetUnitId());
            }
        }
    }
}

void GetCameraPosAndOrt(const Ogre::Vector3 aAvatarPos, Ogre::Vector3& aCameraPos, Ogre::Quaternion& aCameraOrt)
{
    const Ogre::Quaternion tileSpace = Ogre::Vector3::UNIT_Z.getRotationTo(aAvatarPos);
    aCameraPos = aAvatarPos + tileSpace * Ogre::Vector3(0, -40, 50);
    // Camera directed down -z axis!
    const Ogre::Radian angle(Ogre::Math::PI / 5.0f);
    aCameraOrt = tileSpace * Ogre::Quaternion(angle, Ogre::Vector3::UNIT_X);
}

void FreeCameraControl(unsigned long aFrameTime)
{
    const Ogre::Real frameSeconds = FrameTimeToSeconds(aFrameTime);

    const Ogre::Radian rotationSpeed(frameSeconds * 1.0f);
    const Ogre::Real movementSpeed(frameSeconds * 100.0f);

    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_UP))
    {
        ClientApp::GetCamera().pitch(rotationSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_DOWN))
    {
        ClientApp::GetCamera().pitch(-rotationSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_RIGHT))
    {
        ClientApp::GetCamera().yaw(-rotationSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_LEFT))
    {
        ClientApp::GetCamera().yaw(rotationSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_W))
    {
        ClientApp::GetCamera().moveRelative(Ogre::Vector3::NEGATIVE_UNIT_Z * movementSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_S))
    {
        ClientApp::GetCamera().moveRelative(Ogre::Vector3::UNIT_Z * movementSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_D))
    {
        ClientApp::GetCamera().roll(-rotationSpeed);
    }
    if (ClientApp::GetKeyboard().isKeyDown(OIS::KC_A))
    {
        ClientApp::GetCamera().roll(rotationSpeed);
    }
}

void ClientGame::UpdateCamera(unsigned long aFrameTime) const
{
    Ogre::Vector3 pos(Ogre::Vector3::ZERO);
    Ogre::Quaternion ort(Ogre::Quaternion::ZERO);

    if (ClientUnit* avatar = GetUnit(mAvatar))
    {
        GetCameraPosAndOrt(avatar->GetPosition(), pos, ort);
    }

    if (mFreeCamera)
    {
        mAxes->setOrientation(ort);
        mAxes->setPosition(pos);
        FreeCameraControl(aFrameTime);
    }
    else
    {
        ClientApp::GetCamera().setPosition(pos);
        ClientApp::GetCamera().setOrientation(ort);
    }
}


void ClientGame::UpdateStatusPanel(float aAvgFPS) const
{
    GetWindow("StatusPanel/FPS")->setText(Ogre::StringConverter::toString(static_cast<long>(aAvgFPS)));
    GetWindow("StatusPanel/Time")->setText(Ogre::StringConverter::toString(static_cast<long>(mTime)));
    GetWindow("StatusPanel/Ping")->setText(Ogre::StringConverter::toString(mServerProxy->GetPing()));

    int32 lifeTime = FrameTimeToSeconds(mLifeTime);
    if (lifeTime > 0)
    {
        GetWindow("StatusPanel/NetIn")->setText(Ogre::StringConverter::toString(mServerProxy->GetInBytes() / lifeTime));
        GetWindow("StatusPanel/NetOut")->setText(Ogre::StringConverter::toString(mServerProxy->GetOutBytes() / lifeTime));
    }
}

void ClientGame::Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats)
{
    mLifeTime += aFrameTime;

    UpdateCamera(aFrameTime);

    UpdateStatusPanel(aStats.avgFPS);

    std::for_each(mUnits.begin(), mUnits.end(),
                  boost::bind(&ClientUnit::UpdateMovementAnimation,
                              boost::bind(&ClientUnits::value_type::second, _1), aFrameTime));

    if (mSyncTimer.IsTime())
    {
        RequestUpdate();
        mSyncTimer.Reset(mServerUpdateLength);
    }
}

void ClientGame::RequestUpdate()
{
    PayloadPtr req(new PayloadMsg());
    req->set_time(mTime);
    mServerProxy->Request(boost::bind(&ClientGame::OnPayloadMsg, this, _1), req);
}

void ClientGame::OnPayloadMsg(ConstPayloadPtr aPayloadMsg)
{
    if (aPayloadMsg->has_time())
    {
        mTime = aPayloadMsg->time();
    }
    if (aPayloadMsg->has_update_length())
    {
        mServerUpdateLength = aPayloadMsg->update_length();
        mSyncTimer.Reset(mServerUpdateLength);
    }
    LoadEvents(aPayloadMsg);
    if (aPayloadMsg->has_reason())
    {
        LOG(INFO) << aPayloadMsg->ShortDebugString();
    }
}
