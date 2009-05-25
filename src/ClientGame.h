#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <LoadingSheet.h>
#include <InGameSheet.h>
#include <sockio.h>

class ClientApp;

class ClientGame
{
public:
    ClientGame(Ogre::SceneManager& aSceneMgr, QuickGUI::GUIManager& aGUIManager, socket_t& aSocket);
    ~ClientGame();
    ClientGeodesicGrid& GetGrid() { return *mGrid; }
    void UpdateSelectedTilePosition(Ogre::Ray& aRay);
protected:
private:
    Ogre::SceneManager& mSceneMgr;
    QuickGUI::GUIManager& mGUIManager;
    socket_t& mSocket;
    ClientGeodesicGrid* mGrid;
    std::map< int, ClientUnit* > mUnits;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    LoadingSheet mLoadingSheet;
    InGameSheet mIngameSheet;
    void CreateUnitEntities() const;
};

#endif // CLIENTGAME_H
