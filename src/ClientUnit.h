#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>
#include <Typedefs.h>
#include <Unit.pb.h>

class ClientTile;

class ClientUnit
{
public:
    ClientUnit(ClientTile& aTile, UnitMsg& aUnitMsg);
    void SetPosition(ClientTile& aTile);
    ClientTile& GetPosition() const { return *mTile; }
    void SetTarget(ClientTile* aTile);
    ClientTile* GetTarget() const { return mTarget; }
    ~ClientUnit();
    Ogre::Entity* CreateEntity();
    Ogre::SceneNode& GetNode() { return *mNode; }
	UnitId GetUnitId() const { return mUnitId; }
protected:
private:
    ClientTile* mTile;
    ClientTile* mTarget;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
