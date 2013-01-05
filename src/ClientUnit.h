#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H

#include <Ogre.h>
#include <Typedefs.h>
#include <MovementAnimation.h>

class ClientGridNode;

class ClientUnit: public boost::noncopyable
{
public:
    ClientUnit(UnitId aUnitId, uint32 aVisual, ClientGridNode* aTile);
    ~ClientUnit();
    void UpdateMovementAnimation(FrameTime aFrameTime);
    void SetTile(ClientGridNode* aTile);
    ClientGridNode* GetUnitTile() const { return mTile; }
	UnitId GetUnitId() const { return mUnitId; }
private:
    MovementAnimationPtr mMoveAnim;
    Ogre::AnimationState* mAnimState;
    ClientGridNode* mTile;
    Ogre::SceneNode* mPositionNode;
    Ogre::SceneNode* mDirectonNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
