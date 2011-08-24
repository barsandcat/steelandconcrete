#include <pch.h>
#include <ClientUnit.h>

#include <ClientGridNode.h>
#include <ClientApp.h>
#include <VisualCodes.h>
#include <ClientTile.h>

ClientUnit::ClientUnit(UnitId aUnitId, uint32 aVisual, ClientGridNode* aTile):
    mTile(aTile),
    mEntity(NULL),
    mUnitId(aUnitId),
    mVisualCode(aVisual),
    mPositionNode(NULL),
    mDirectonNode(NULL)
{
    assert(mTile);
    mTile->SetUnit(this);
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
}

ClientUnit::~ClientUnit()
{
    ClientGame::EraseUnitId(mUnitId);
    ClientApp::GetSceneMgr().destroyEntity(mEntity);
    ClientApp::GetSceneMgr().destroySceneNode(mPositionNode);
    ClientApp::GetSceneMgr().destroySceneNode(mDirectonNode);
    mTile->RemoveUnit();
}

void ClientUnit::UpdateMovementAnimation(FrameTime aFrameTime)
{
    if (mMoveAnim)
    {
        mMoveAnim->Update(aFrameTime);
        const Ogre::Quaternion orientation = mMoveAnim->GetPosition();
        mPositionNode->setOrientation(orientation);
        if (mMoveAnim->IsDone())
        {
            mMoveAnim.reset();
        }
    }
}

void ClientUnit::SetTile(ClientGridNode* aTile)
{
    assert(aTile);
    const Ogre::Vector3 origin = mTile->GetPosition();
    const Ogre::Vector3 destin = aTile->GetPosition();
    mMoveAnim.reset(new MovementAnimation(origin, destin));

    const Ogre::Vector3 dir = destin - origin;
    mDirectonNode->setDirection(dir, Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Y);

    mTile->RemoveUnit();
    aTile->SetUnit(this);
    mTile = aTile;
}
