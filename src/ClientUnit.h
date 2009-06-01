#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>
#include <Typedefs.h>

class ClientTile;

class ClientUnit
{
public:
    ClientUnit(ClientTile& aTile, UnitId aUnitId);
    void SetPosition(ClientTile& aTile);
    ~ClientUnit();
    Ogre::Entity* CreateEntity();
protected:
private:
    ClientTile* mTile;
    Ogre::SceneNode* mNode;
    UnitId mUnitId;
};

#endif // CLIENTUNIT_H
