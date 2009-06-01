#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <LoadingSheet.h>
#include <InGameSheet.h>
#include <sockio.h>
#include <ChangeList.pb.h>
#include <Typedefs.h>

class ClientApp;

class ClientGame
{
public:
    ClientGame(socket_t& aSocket);
    virtual ~ClientGame(); // Для QuicGUI
    void OnExit(const QuickGUI::EventArgs& args);
    void OnTurn(const QuickGUI::EventArgs& args);
    void UpdateSelectedTilePosition(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime);
protected:
private:
    socket_t& mSocket;
    ClientGeodesicGrid* mGrid;
    std::map< UnitId, ClientUnit* > mUnits;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    LoadingSheet mLoadingSheet;
    InGameSheet mIngameSheet;
    int mTime;
    bool mTurnDone;
    void CreateUnitEntities() const;
    void LoadEvents(const ChangeListMsg& changes);
};

#endif // CLIENTGAME_H
