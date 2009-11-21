#include <pch.h>
#include <ClientUnit.h>

#include <ClientTile.h>
#include <ClientApp.h>
#include <VisualCodes.h>

ClientUnit::ClientUnit(ClientTile& aTile, UnitMsg& aUnitMsg):
    mTile(&aTile),
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
    //dtor
}

Ogre::Entity* ClientUnit::CreateEntity()
{
    Ogre::String indexName = Ogre::StringConverter::toString(mUnitId);
    Ogre::Entity* unit = ClientApp::GetSceneMgr().createEntity(indexName + "Unit.entity", GetMesh(mVisualCode));
    mNode->attachObject(unit);
    mNode->setVisible(true);
    return unit;
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
