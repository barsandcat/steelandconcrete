#include <pch.h>
#include <ClientUnit.h>

#include <ClientTile.h>
#include <ClientApp.h>
#include <VisualCodes.h>
#include <TileEntity.h>

const char* WALK_ANIMATION_NAME = "walk";

ClientUnit::ClientUnit(UnitId aUnitId, uint32 aVisual, ClientTile* aTile):
    mTile(aTile),
    mEntity(NULL),
    mUnitId(aUnitId),
    mVisualCode(aVisual),
    mPositionNode(NULL),
    mDirectonNode(NULL),
    mAnimState(NULL)
{
    assert(mTile);
    mTile->SetUnit(aUnitId);
    const Ogre::Vector3 pos = mTile->GetPosition();

    mPositionNode = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    const Ogre::Quaternion orientation = Ogre::Vector3::UNIT_Z.getRotationTo(pos);
    mPositionNode->setOrientation(orientation);
    mDirectonNode = mPositionNode->createChildSceneNode();
    mDirectonNode->translate(pos, Ogre::Node::TS_WORLD);
    mDirectonNode->setFixedYawAxis(true, pos);

    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    mEntity = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
    mDirectonNode->attachObject(mEntity);
    mDirectonNode->setVisible(true);
    if (mEntity->hasSkeleton())
    {
        mAnimState = mEntity->getAnimationState(WALK_ANIMATION_NAME);
        mAnimState->setLoop(true);
    }
}

ClientUnit::~ClientUnit()
{
    ClientApp::GetSceneMgr().destroyEntity(mEntity);
    ClientApp::GetSceneMgr().destroySceneNode(mPositionNode);
    ClientApp::GetSceneMgr().destroySceneNode(mDirectonNode);
}

void ClientUnit::UpdateMovementAnimation(Miliseconds aFrameTime)
{
    if (mMoveAnim)
    {
        mMoveAnim->Update(aFrameTime);
        const Ogre::Quaternion orientation = mMoveAnim->GetPosition();
        mPositionNode->setOrientation(orientation);
        mAnimState->addTime(FrameTimeToSeconds(aFrameTime));
        if (mMoveAnim->IsDone())
        {
            mMoveAnim.reset();
            if (mAnimState)
            {
               mAnimState->setEnabled(false);
            }
        }
    }
}

void ClientUnit::SetTile(ClientTile* aTile)
{
    assert(aTile);
    if (aTile == mTile)
    {
        return;
    }

    const Ogre::Vector3 origin = mTile->GetPosition();
    const Ogre::Vector3 destin = aTile->GetPosition();
    mMoveAnim.reset(new MovementAnimation(origin, destin));
    if (mAnimState)
    {
        mAnimState->setEnabled(true);
    }

    const Ogre::Vector3 dir = destin - origin;
    mDirectonNode->setFixedYawAxis(true, origin);
    mDirectonNode->setDirection(dir, Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Y);

    mTile->RemoveUnit();
    aTile->SetUnit(mUnitId);
    mTile = aTile;
}

Ogre::Vector3 ClientUnit::GetPosition() const
{
    return mPositionNode->convertLocalToWorldPosition(mDirectonNode->getPosition());
}
