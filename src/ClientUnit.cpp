#include <pch.h>
#include <ClientUnit.h>

#include <ClientTile.h>
#include <ClientApp.h>
#include <VisualCodes.h>

ClientUnit::ClientUnit(ClientTile& aTile, UnitMsg& aUnitMsg):
    mTile(&aTile),
    mEntity(NULL),
    mTarget(NULL),
    mUnitId(aUnitMsg.tag()),
    mVisualCode(aUnitMsg.visual())
{
    mTile->SetUnit(this);
    mNode = mTile->GetNode().createChildSceneNode();
    mNode->setScale(Ogre::Vector3(0.1));
}

ClientUnit::~ClientUnit()
{
    delete mEntity;
    delete mNode;
}

Ogre::Entity* ClientUnit::CreateEntity()
{
    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    Ogre::Entity* mEntity = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
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
