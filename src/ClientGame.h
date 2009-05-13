#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <sockio.h>

class ClientApp;

class ClientGame
{
public:
    ClientGame(ClientApp& aApp, socket_t& aSocket);
    ~ClientGame();
    ClientGeodesicGrid& GetGrid() { return mGrid; }
    ClientApp& GetApp() { return mApp; }
    void CreateUnitEntities(Ogre::SceneManager& aSceneManager) const;
protected:
private:
    ClientApp& mApp;
    socket_t& mSocket;
    ClientGeodesicGrid mGrid;
    std::map< int, ClientUnit* > mUnits;
};

#endif // CLIENTGAME_H
