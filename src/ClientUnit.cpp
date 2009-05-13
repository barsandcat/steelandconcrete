#include <ClientUnit.h>
#include <ClientTile.h>

ClientUnit::ClientUnit(ClientTile& aTile, int aIndex): mTile(&aTile), mIndex(aIndex)
{
    //ctor
}

ClientUnit::~ClientUnit()
{
    //dtor
}

Ogre::Entity* ClientUnit::CreateEntity(Ogre::SceneManager& aSceneManager)
{
    Ogre::String indexName = Ogre::StringConverter::toString(mIndex);
    Ogre::Entity* unit = aSceneManager.createEntity(indexName + "Unit.entity", "robot.mesh");
    Ogre::SceneNode* node = aSceneManager.getRootSceneNode()->createChildSceneNode(indexName + "Unit.node");
    node->setScale(Ogre::Vector3(0.0011));
    node->setPosition(mTile->GetPosition());
    node->setDirection(mTile->GetPosition(), Ogre::SceneNode::TS_LOCAL, Ogre::Vector3::UNIT_Y);
    node->attachObject(unit);
    node->setVisible(true);
    return unit;
}
