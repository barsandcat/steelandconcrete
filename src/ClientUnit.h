#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>
#include <Typedefs.h>
#include <Unit.pb.h>

class ClientTile;

class ClientUnit: public boost::noncopyable
{
public:
    ClientUnit(UnitMsg& aUnitMsg);
    void SetTile(ClientTile* aTile);
    ClientTile* GetTile() const { return mTile; }
    void SetTarget(ClientTile* aTile);
    ClientTile* GetTarget() const { return mTarget; }
    ~ClientUnit();
    Ogre::Entity* CreateEntity();
    Ogre::SceneNode& GetNode() { return *mNode; }
	UnitId GetUnitId() const { return mUnitId; }
private:
    ClientTile* mTile;
    ClientTile* mTarget;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
