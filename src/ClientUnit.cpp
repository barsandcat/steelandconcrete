#include <pch.h>
#include <ClientUnit.h>

#include <ClientGridNode.h>
#include <ClientApp.h>
#include <VisualCodes.h>
#include <ClientTile.h>

ClientUnit::ClientUnit(UnitId aUnitId, uint32 aVisual, ClientTile* aTile):
    mTile(aTile),
    mEntity(NULL),
    mUnitId(aUnitId),
    mVisualCode(aVisual)
{
    assert(mTile);
    const Ogre::Vector3 pos = mTile->GetPosition();

    mNode = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    //const Ogre::Quaternion orientation = pos.getRotationTo(Ogre::Vector3::UNIT_Z);
    //mNode->setOrientation(orientation);
    mNode->setDirection(pos.normalisedCopy(), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    Ogre::SceneNode* unitNode = mNode->createChildSceneNode();
    unitNode->translate(pos, Ogre::Node::TS_WORLD);

    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    mEntity = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
    unitNode->attachObject(mEntity);
    unitNode->setVisible(true);
}

ClientUnit::~ClientUnit()
{
    ClientGame::EraseUnitId(mUnitId);
    ClientApp::GetSceneMgr().destroyEntity(mEntity);
    ClientApp::GetSceneMgr().destroySceneNode(mNode);
    mTile->RemoveUnit();
    mEntity = NULL;
    mNode = NULL;
}

void ClientUnit::UpdateMovementAnimation(FrameTime aFrameTime)
{
    if (mMoveAnim)
    {
        mMoveAnim->Update(aFrameTime);
        const Ogre::Quaternion orientation = mMoveAnim->GetPosition();
        mNode->setOrientation(orientation);
        if (mMoveAnim->IsDone())
        {
            mMoveAnim.reset();
        }
    }
}

void ClientUnit::SetTile(ClientTile* aTile)
{
    assert(aTile);
    //mMoveAnim.reset(new MovementAnimation(mTile->GetPosition(), aTile->GetPosition()));
    mTile->RemoveUnit();
    aTile->SetUnit(this);
    mTile = aTile;
    const Ogre::Vector3 pos = mTile->GetPosition();
    mNode->setDirection(pos.normalisedCopy(), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
}
