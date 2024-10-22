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
    void UpdateMovementAnimation(Miliseconds aFrameTime);
    void SetTile(ClientTile* aTile);
    ClientTile& GetUnitTile() const { return *mTile; }
	UnitId GetUnitId() const { return mUnitId; }
	Ogre::Vector3 GetPosition() const;
private:
    MovementAnimationPtr mMoveAnim;
    Ogre::AnimationState* mAnimState;
    ClientTile* mTile;
    Ogre::SceneNode* mPositionNode;
    Ogre::SceneNode* mDirectonNode;
    Ogre::Entity* mEntity;
    const UnitId mUnitId;
    const uint32 mVisualCode;
};

#endif // CLIENTUNIT_H
