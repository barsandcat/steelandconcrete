#include <pch.h>
#include <ClientUnit.h>

#include <ClientTile.h>
#include <ClientApp.h>
#include <VisualCodes.h>

ClientUnit::ClientUnit(ClientTile& aTile, UnitMsg& aUnitMsg):
    mTile(&aTile),
    mTarget(NULL),
    mEntity(NULL),
    mUnitId(aUnitMsg.tag()),
    mVisualCode(aUnitMsg.visual())
{
    mTile->SetUnit(this);
    mNode = mTile->GetNode().createChildSceneNode();
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

void ClientUnit::SetPosition(ClientTile& aTile)
{
    mTile->SetUnit(NULL);
    mTile->GetNode().removeChild(mNode);
    aTile.SetUnit(this);
    aTile.GetNode().addChild(mNode);
    mTile = &aTile;
}

void ClientUnit::SetTarget(ClientTile* aTile)
{
    mTarget = aTile;
}
