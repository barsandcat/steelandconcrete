#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <LoadingSheet.h>
#include <InGameSheet.h>
#include <Response.pb.h>
#include <Typedefs.h>
#include <Network.h>
#include <SyncTimer.h>
#include <SystemMenuSheet.h>

class ClientApp;

class ClientGame
{
public:
    typedef std::map< UnitId, ClientUnit* > ClientUnits;
    ClientGame(Network* aNetwork, UnitId aAvatar, int32 aGridSize);
    virtual ~ClientGame(); // Для QuicGUI
    void UpdateTileUnderCursor(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats);
    void OnExit(const QuickGUI::EventArgs& args);
    void OnEscape();
    void OnAct();
private:
    void CreateUnitEntities() const;
    int32 ReadResponseMessage();
    void LoadEvents(const ResponseMsg& changes);
    ClientUnit& GetUnit(UnitId aUnitId);
private:
    ClientGeodesicGrid::Tiles mTiles;
    ClientUnits mUnits;
    ClientGridNode* mTileUnderCursor;
    ClientUnit* mAvatar;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneNode* mTargetMarker;
    LoadingSheet mLoadingSheet;
    InGameSheet mIngameSheet;
    SystemMenuSheet mSystemMenuSheet;
    GameTime mTime;
    SyncTimer mSyncTimer;
    Network* mNetwork;
};

#endif // CLIENTGAME_H
