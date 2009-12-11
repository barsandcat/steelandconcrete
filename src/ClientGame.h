#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <LoadingSheet.h>
#include <InGameSheet.h>
#include <sockio.h>
#include <Response.pb.h>
#include <Typedefs.h>
#include <ViewPortWidget.h>
#include <Network.h>

class ClientApp;

class ClientGame
{
public:
    typedef std::map< UnitId, ClientUnit* > ClientUnits;
    ClientGame(Network* aNetwork);
    virtual ~ClientGame(); // Для QuicGUI
    void OnExit(const QuickGUI::EventArgs& args);
    void OnTurn(const QuickGUI::EventArgs& args);
    void UpdateTileUnderCursor(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime, const Ogre::RenderTarget::FrameStats& aStats);
    void Select();
    void Act();
protected:
private:
    ClientGeodesicGrid* mGrid;
    ClientUnits mUnits;
    ClientTile* mTileUnderCursor;
    ClientUnit* mSelectedUnit;
    ClientUnit* mAvatar;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneNode* mTargetMarker;
    LoadingSheet mLoadingSheet;
    InGameSheet mIngameSheet;
    ViewPortWidget mViewPortWidget;
    GameTime mTime;
    bool mTurnDone;
    Network* mNetwork;
    void CreateUnitEntities() const;
    void LoadEvents(const ResponseMsg& changes);
};

#endif // CLIENTGAME_H
