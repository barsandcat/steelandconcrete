#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <Response.pb.h>
#include <Typedefs.h>
#include <Network.h>
#include <SyncTimer.h>
#include <CEGUI.h>

class ClientApp;

class ClientGame
{
public:
    typedef std::map< UnitId, ClientUnit* > ClientUnits;
    ClientGame(Network* aNetwork, UnitId aAvatar, int32 aGridSize);
    virtual ~ClientGame(); // Для QuicGUI
    void UpdateTileUnderCursor(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats);
    bool OnExit(const CEGUI::EventArgs& args);
    void OnEscape();
    void OnAct();
private:
    void OnResponseMsg(ResponsePtr aResponseMsg);
    void CreateUnitEntities() const;
    void LoadAvatar();
    void LoadEvents(ResponsePtr aResponseMsg);
    ClientUnit& GetUnit(UnitId aUnitId);
private:
    ClientGeodesicGrid::Tiles mTiles;
    ClientUnits mUnits;
    ClientGridNode* mTileUnderCursor;
    ClientUnit* mAvatar;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneNode* mTargetMarker;
    GameTime mTime;
    SyncTimer mSyncTimer;
    Network* mNetwork;
    std::list< boost::shared_ptr<ResponseMsg> > mMessages;
};

#endif // CLIENTGAME_H
