#include <pch.h>
#include <ClientUnit.h>

#include <ClientGridNode.h>
#include <ClientApp.h>
#include <VisualCodes.h>
#include <ClientTile.h>

ClientUnit::ClientUnit(ClientGridNode& aTile, UnitMsg& aUnitMsg):
    mTile(&aTile),
    mTarget(NULL),
    mEntity(NULL),
    mUnitId(aUnitMsg.tag()),
    mVisualCode(aUnitMsg.visual())
{
    mTile->GetTile()->SetUnit(this);
    mNode = mTile->GetTile()->GetNode().createChildSceneNode();
}

ClientUnit::~ClientUnit()
{
    ClientApp::GetSceneMgr().destroyEntity(mEntity);
    ClientApp::GetSceneMgr().destroySceneNode(mNode);
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

void ClientUnit::SetPosition(ClientGridNode& aTile)
{
    mTile->GetTile()->SetUnit(NULL);
    mTile->GetTile()->GetNode().removeChild(mNode);
    aTile.GetTile()->SetUnit(this);
    aTile.GetTile()->GetNode().addChild(mNode);
    mTile = &aTile;
}

void ClientUnit::SetTarget(ClientGridNode* aTile)
{
    mTarget = aTile;
}
