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

Ogre::Entity* ClientUnit::CreateEntity()
{
    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    mEntity = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
    mNode->attachObject(mEntity);
    mNode->setVisible(true);
    return mEntity;
}

void ClientUnit::SetTile(ClientTile* aTile)
{
    if (mTile)
    {
        mTile->SetUnit(NULL);
        mTile = NULL;
    }

    Ogre::SceneNode* parent = mNode->getParentSceneNode();
    if (parent)
    {
        parent->removeChild(mNode);
    }

    if (aTile)
    {
        aTile->SetUnit(this);
        aTile->GetNode().addChild(mNode);
        mTile = aTile;
    }
}
