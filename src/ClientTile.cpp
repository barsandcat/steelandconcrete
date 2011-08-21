#include <pch.h>
#include <ClientTile.h>

#include <ClientApp.h>
#include <CompareEdgesAngles.h>

ClientTile::ClientTile(bool ground, ClientGridNode& aGridNode):
        mNode(NULL),
        mGridNode(aGridNode),
        mGround(ground),
        mUnit(NULL),
        mEntity(NULL)
{
    Ogre::SceneManager& aSceneManager = ClientApp::GetSceneMgr();
    Ogre::SceneNode* root = aSceneManager.getRootSceneNode();
    mNode = root->createChildSceneNode(aGridNode.GetPosition());
    mNode->setDirection(aGridNode.GetPosition().normalisedCopy(), Ogre::Node::TS_LOCAL, Ogre::Vector3::UNIT_Z);

    Ogre::String indexName = Ogre::StringConverter::toString(mGridNode.GetTileId());
    Ogre::String meshName = indexName + "ClientTile.mesh";
    // Create entity
    Ogre::MeshPtr tileMesh = ConstructMesh(meshName);
    mEntity = aSceneManager.createEntity(indexName + "ClientTile.entity", meshName);
    root->attachObject(mEntity);
}

ClientTile::~ClientTile()
{
    Ogre::SceneManager& aSceneManager = ClientApp::GetSceneMgr();
    aSceneManager.getRootSceneNode()->detachObject(mEntity);
    aSceneManager.destroyEntity(mEntity);
    aSceneManager.destroySceneNode(mNode);

    if (mUnit)
    {
        mUnit->SetTile(NULL);
        delete mUnit;
    }
}

Ogre::Vector3 ClientTile::GetPosition() const
{
    return mGridNode.GetPosition();
}


Ogre::MeshPtr ClientTile::ConstructMesh(const Ogre::String& aMeshName) const
{
    const Ogre::Real pentagonHorizont = 0.5f / tan(Ogre::Math::DegreesToRadians(108.0f / 2.0f));
    const Ogre::Real pentagonBottomStep = (1.0f - 0.5f / sin(Ogre::Math::DegreesToRadians(108.0f / 2.0f))) / 2.0f;
    const Ogre::Real hexagonStep = 0.5f / tan(Ogre::Math::DegreesToRadians(120.0f / 2.0f));

    Ogre::ManualObject manual("Manual" + aMeshName);

    //    0-----0.5-----1
    // 0         1
    // |
    // |  0             2 - pentagonHorizont
    // |         X
    // |
    // 1     4       3
    //    ^--bottomStep-^

    //    0---0.5---1
    // 0       1      hexagonStep
    // |  0         2
    // |       X
    // |  5         3
    // 1       4      1 - hexagonStep
    //

    bool pentagon = mGridNode.GetNeighbourCount() == 5;

    Ogre::Vector3 point;
    Ogre::String material;
    if (mGround)
    {
        material = "Grassland";
    }
    else
    {
        material = "Ocean";
    }

    manual.begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);

    assert(mGridNode.GetNeighbourCount() > 4);

    const Ogre::Vector3 positon = mGridNode.GetPosition();
    const Ogre::Vector3 normal = positon.normalisedCopy();
    const Ogre::Real len = positon.length();

    // Vetex setup
    //0
    manual.position((positon + mGridNode.GetNeighbour(0).GetPosition() + mGridNode.GetNeighbour(1).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 0);
    //1
    manual.position((positon + mGridNode.GetNeighbour(1).GetPosition() + mGridNode.GetNeighbour(2).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(0, 0.5);
    //2
    manual.position((positon + mGridNode.GetNeighbour(2).GetPosition() + mGridNode.GetNeighbour(3).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 1);
    //3
    manual.position((positon + mGridNode.GetNeighbour(3).GetPosition() + mGridNode.GetNeighbour(4).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(1.0f - (pentagon ? 0 : hexagonStep), 1.0f - (pentagon ? pentagonBottomStep : 0));

    if (pentagon)
    {
        //4
        manual.position((positon + mGridNode.GetNeighbour(4).GetPosition() + mGridNode.GetNeighbour(0).GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, pentagonBottomStep);
    }
    else
    {
        //4
        manual.position((positon + mGridNode.GetNeighbour(4).GetPosition() + mGridNode.GetNeighbour(5).GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, 0.5f);
        //5
        manual.position((positon + mGridNode.GetNeighbour(5).GetPosition() + mGridNode.GetNeighbour(0).GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f - hexagonStep, 0.0f);
    }

    // Making triangles
    manual.triangle(0, 2, 1);
    manual.triangle(0, 3, 2);
    manual.triangle(0, 4, 3);
    if (!pentagon)
    {
        manual.triangle(0, 5, 4);
    }

    manual.end();

    return manual.convertToMesh(aMeshName);
}
