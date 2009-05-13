#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>

class ClientTile;

class ClientUnit
{
public:
    ClientUnit(ClientTile& aTile, int aIndex);
    ~ClientUnit();
    Ogre::Entity* CreateEntity(Ogre::SceneManager& aSceneManager);
protected:
private:
    ClientTile* mTile;
    int mIndex;
};

#endif // CLIENTUNIT_H
