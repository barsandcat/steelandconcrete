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
    mTile->RemoveUnit();
    mEntity = NULL;
    mNode = NULL;
}

void ClientUnit::SetTile(ClientTile* aTile)
{
    assert(aTile);
    mTile->RemoveUnit();
    aTile->SetUnit(this);
    mTile = aTile;
    const Ogre::Vector3 pos = aTile->GetGridNode().GetPosition();
    mNode->setDirection(pos.normalisedCopy(), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    mNode->setPosition(pos);
}
