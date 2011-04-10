#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>
#include <Typedefs.h>
#include <Unit.pb.h>

class ClientGridNode;

class ClientUnit
{
public:
    ClientUnit(ClientGridNode& aTile, UnitMsg& aUnitMsg);
    void SetPosition(ClientGridNode& aTile);
    ClientGridNode& GetPosition() const { return *mTile; }
    void SetTarget(ClientGridNode* aTile);
    ClientGridNode* GetTarget() const { return mTarget; }
    ~ClientUnit();
    Ogre::Entity* CreateEntity();
    Ogre::SceneNode& GetNode() { return *mNode; }
	UnitId GetUnitId() const { return mUnitId; }
protected:
private:
    ClientGridNode* mTile;
    ClientGridNode* mTarget;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
