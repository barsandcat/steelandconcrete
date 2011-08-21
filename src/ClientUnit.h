#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H

#include <Ogre.h>
#include <Typedefs.h>
#include <MovementAnimation.h>

class ClientTile;

class ClientUnit: public boost::noncopyable
{
public:
    ClientUnit(UnitId aUnitId, uint32 aVisual, ClientTile* aTile);
    ~ClientUnit();
    void UpdateMovementAnimation(FrameTime aFrameTime);
    void SetTile(ClientTile* aTile);
    ClientTile* GetTile() const { return mTile; }
	UnitId GetUnitId() const { return mUnitId; }
private:
    MovementAnimationPtr mMoveAnim;
    ClientTile* mTile;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
