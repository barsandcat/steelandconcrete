#include <pch.h>
#include <ClientUnit.h>

#include <ClientGridNode.h>
#include <ClientApp.h>
#include <VisualCodes.h>
#include <ClientTile.h>

ClientUnit::ClientUnit(UnitId aUnitId, uint32 aVisual):
    mTile(NULL),
    mEntity(NULL),
    mUnitId(aUnitId),
    mVisualCode(aVisual)
{
    mNode = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode();
    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    mEntity = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
    mNode->attachObject(mEntity);
    mNode->setVisible(true);
}

ClientUnit::~ClientUnit()
{
    ClientGame::EraseUnitId(mUnitId);
    ClientApp::GetSceneMgr().destroyEntity(mEntity);
    ClientApp::GetSceneMgr().destroySceneNode(mNode);
    if (mTile)
    {
        mTile->SetUnit(NULL);
    }
    mEntity = NULL;
    mNode = NULL;
}

void ClientUnit::SetTile(ClientTile* aTile)
{
    if (mTile)
    {
        mTile->SetUnit(NULL);
        mTile = NULL;
    }

    if (aTile)
    {
        aTile->SetUnit(this);
        mTile = aTile;
        const Ogre::Vector3 pos = aTile->GetGridNode().GetPosition();
        mNode->setDirection(pos.normalisedCopy(), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
        mNode->setPosition(pos);
    }

}
