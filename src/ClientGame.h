#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <Payload.pb.h>
#include <Typedefs.h>
#include <Network.h>
#include <SyncTimer.h>
#include <CEGUI.h>

class ClientApp;

class ClientGame
{
public:
    typedef std::map< UnitId, ClientUnit* > ClientUnits;
    ClientGame(NetworkPtr aNetwork, UnitId aAvatar, int32 aGridSize);
    virtual ~ClientGame(); // Для QuicGUI
    void UpdateTileUnderCursor(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats);
    bool OnExit(const CEGUI::EventArgs& args);
    void OnEscape();
    void OnAct();
    static ClientUnit* GetUnit(UnitId aUnitId);
    static void EraseUnitId(UnitId aUnitId);
private:
    void OnPayloadMsg(PayloadPtr aPayloadMsg);
    void LoadAvatar();
    void LoadEvents(PayloadPtr aPayloadMsg);
private:
    static ClientUnits mUnits;
    ClientGeodesicGrid::Tiles mTiles;
    ClientGridNode* mTileUnderCursor;
    ClientUnit* mAvatar;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneNode* mTargetMarker;
    GameTime mTime;
    SyncTimer mSyncTimer;
    int32 mServerUpdateLength;
    NetworkPtr mNetwork;
};

#endif // CLIENTGAME_H
