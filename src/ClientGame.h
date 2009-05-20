#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <sockio.h>

class ClientApp;

class ClientGame
{
public:
    ClientGame(Ogre::SceneManager& aSceneMgr, socket_t& aSocket);
    ~ClientGame();
    ClientGeodesicGrid& GetGrid() { return mGrid; }
    void UpdateSelectedTilePosition(Ogre::Ray& aRay);
protected:
private:
    Ogre::SceneManager& mSceneMgr;
    socket_t& mSocket;
    ClientGeodesicGrid mGrid;
    std::map< int, ClientUnit* > mUnits;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    void CreateUnitEntities() const;
};

#endif // CLIENTGAME_H
