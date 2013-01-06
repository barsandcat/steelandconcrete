#include <pch.h>
#include <ClientGame.h>

#include <ClientApp.h>
#include <ServerProxy.h>
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

ClientGame::ClientGame(ServerProxyPtr aServerProxy, TileId aLandingTileId, int32 aGridSize):
    mTileUnderCursor(NULL),
    mTime(0),
    mSyncTimer(1000),
    mServerUpdateLength(1000),
    mServerProxy(aServerProxy)
{
    ClientGeodesicGrid grid(mTiles, aGridSize);

    Ogre::Vector3 avatarPosition = mTiles.at(aLandingTileId)->GetPosition();
    mBirdCamera = new BirdCamera(avatarPosition, avatarPosition.length(), avatarPosition.length() + 200.0f);

    // Create a light
    Ogre::Light* myLight = ClientApp::GetSceneMgr().createLight("Light0");
    myLight->setType(Ogre::Light::LT_DIRECTIONAL);
    myLight->setPosition(50, 0, 0);
    myLight->setDirection(-1, 0, 0);
    myLight->setDiffuseColour(1, 1, 1);
    myLight->setSpecularColour(1, 1, 1);

    mTileUnderCursor = mTiles.at(aLandingTileId);
    mSelectionMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mSelectionMarker->setScale(Ogre::Vector3(0.1));
    mSelectionMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Marker", Ogre::SceneManager::PT_SPHERE));

    mTargetMarker = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    mTargetMarker->attachObject(ClientApp::GetSceneMgr().createEntity("Target", "TargetMarker.mesh"));
    mTargetMarker->setVisible(false);

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* guiRoot = winMgr.loadWindowLayout("Game.layout", "", "", &PropertyCallback);
    winMgr.getWindow("Game/InGameMenu/Exit")->
    subscribeEvent(CEGUI::PushButton::EventClicked,
                   CEGUI::Event::Subscriber(&ClientGame::OnExit, this));

    CEGUI::System::getSingleton().setGUISheet(guiRoot);

    RequestUpdate();
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
    if (arg.state.X.abs >= arg.state.width && arg.state.X.rel > 0 ||
            arg.state.X.abs <= 0 && arg.state.X.rel < 0)
    {
        mBirdCamera->SetHorizontalSpeed(arg.state.X.rel);
    }

    if (arg.state.Y.abs >= arg.state.height && arg.state.Y.rel > 0 ||
            arg.state.Y.abs <= 0 && arg.state.Y.rel < 0 )
    {
        mBirdCamera->SetVerticalSpeed(arg.state.Y.rel);
    }


}
void ClientGame::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    switch (id)
    {
    case OIS::MB_Left:
        break;
    case OIS::MB_Right:
        OnAct();
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
    case OIS::KC_W:
        break;
    case OIS::KC_S:
        break;
    case OIS::KC_A:
        break;
    case OIS::KC_D:
        break;
    case OIS::KC_SUBTRACT:
    case OIS::KC_MINUS:
        mBirdCamera->ZoomOut();
        break;
    case OIS::KC_ADD:
    case OIS::KC_EQUALS:
        mBirdCamera->ZoomIn();
        break;
    case OIS::KC_ESCAPE:
        OnEscape();
        break;
    default:
        ;
    }
}
void ClientGame::keyReleased(const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
    case OIS::KC_W:
        break;
    case OIS::KC_S:
        break;
    case OIS::KC_A:
        break;
    case OIS::KC_D:
        break;
    case OIS::KC_SUBTRACT:
    case OIS::KC_MINUS:
        mBirdCamera->ZoomIn();
        break;
    case OIS::KC_ADD:
    case OIS::KC_EQUALS:
        mBirdCamera->ZoomOut();
        break;
    default:
        ;
    }


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

bool ClientGame::OnExit(const CEGUI::EventArgs& args)
{
    boost::throw_exception(std::runtime_error("Exit"));
    return true;
}

void ClientGame::OnEscape()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* inGameMenu = winMgr.getWindow("Game/InGameMenu");
    inGameMenu->setVisible(!inGameMenu->isVisible());
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
                    ClientUnit* unit = new ClientUnit(move.unitid(), move.visualcode(), mTiles.at(move.to()));
                    mUnits.insert(std::make_pair(move.unitid(), unit));
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
            delete node->GetUnit();
        }
    }
}

void ClientGame::Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats)
{
    mBirdCamera->UpdatePosition(aFrameTime);

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.getWindow("Game/FPS")->setText(Ogre::StringConverter::toString(aStats.avgFPS));
    winMgr.getWindow("Game/Time")->setText(Ogre::StringConverter::toString(static_cast<long>(mTime)));

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
