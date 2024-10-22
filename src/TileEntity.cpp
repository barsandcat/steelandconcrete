#include <pch.h>
#include <TileEntity.h>

#include <ClientApp.h>
#include <CompareEdgesAngles.h>

TileEntity::TileEntity(bool ground, ClientTile& aTile):
        mNode(NULL),
        mTile(aTile),
        mGround(ground),
        mEntity(NULL)
{
    Ogre::SceneManager& aSceneManager = ClientApp::GetSceneMgr();
    Ogre::SceneNode* root = aSceneManager.getRootSceneNode();
    mNode = root->createChildSceneNode(aTile.GetPosition());
    mNode->setDirection(aTile.GetPosition().normalisedCopy(), Ogre::Node::TS_LOCAL, Ogre::Vector3::UNIT_Z);

    Ogre::String indexName = Ogre::StringConverter::toString(aTile.GetTileId());
    Ogre::String meshName = indexName + "TileEntity.mesh";
    // Create entity
    Ogre::MeshPtr tileMesh = ConstructMesh(meshName);
    mEntity = aSceneManager.createEntity(indexName + "ClientTile.entity", meshName);
    root->attachObject(mEntity);
}

TileEntity::~TileEntity()
{
    Ogre::SceneManager& aSceneManager = ClientApp::GetSceneMgr();
    aSceneManager.getRootSceneNode()->detachObject(mEntity);
    aSceneManager.destroyEntity(mEntity);
    aSceneManager.destroySceneNode(mNode);
}

Ogre::Vector3 TileEntity::GetPosition() const
{
    return mTile.GetPosition();
}


Ogre::MeshPtr TileEntity::ConstructMesh(const Ogre::String& aMeshName) const
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

    bool pentagon = mTile.GetNeighbourCount() == 5;

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

    assert(mTile.GetNeighbourCount() > 4);

    const Ogre::Vector3 positon = mTile.GetPosition();
    const Ogre::Vector3 normal = positon.normalisedCopy();
    const Ogre::Real len = positon.length();

    // Vetex setup
    //0
    manual.position((positon + mTile.GetNeighbour(0).GetPosition() + mTile.GetNeighbour(1).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 0);
    //1
    manual.position((positon + mTile.GetNeighbour(1).GetPosition() + mTile.GetNeighbour(2).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(0, 0.5);
    //2
    manual.position((positon + mTile.GetNeighbour(2).GetPosition() + mTile.GetNeighbour(3).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 1);
    //3
    manual.position((positon + mTile.GetNeighbour(3).GetPosition() + mTile.GetNeighbour(4).GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(1.0f - (pentagon ? 0 : hexagonStep), 1.0f - (pentagon ? pentagonBottomStep : 0));

    if (pentagon)
    {
        //4
        manual.position((positon + mTile.GetNeighbour(4).GetPosition() + mTile.GetNeighbour(0).GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, pentagonBottomStep);
    }
    else
    {
        //4
        manual.position((positon + mTile.GetNeighbour(4).GetPosition() + mTile.GetNeighbour(5).GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, 0.5f);
        //5
        manual.position((positon + mTile.GetNeighbour(5).GetPosition() + mTile.GetNeighbour(0).GetPosition()).normalisedCopy() * len);
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
