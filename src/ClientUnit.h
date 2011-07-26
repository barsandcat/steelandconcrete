#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H
#include <Ogre.h>
#include <Typedefs.h>

class ClientTile;

class ClientUnit: public boost::noncopyable
{
public:
    ClientUnit(UnitId aUnitId, uint32 aVisual);
    void SetTile(ClientTile* aTile);
    ClientTile* GetTile() const { return mTile; }
    ~ClientUnit();
    Ogre::Entity* CreateEntity();
    Ogre::SceneNode& GetNode() { return *mNode; }
	UnitId GetUnitId() const { return mUnitId; }
private:
    ClientTile* mTile;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
