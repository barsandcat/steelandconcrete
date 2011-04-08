#include <pch.h>
#include <ClientTile.h>

#include <ClientApp.h>
#include <CompareEdgesAngles.h>

ClientTile::ClientTile(TileId aId, float scale, bool ground, const Ogre::Vector3& aPosition):
        mNode(*ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode(aPosition)),
        mTileId(aId),
        mGround(ground),
        mUnit(NULL)
{
    mNeighbourhood.reserve(6);
    //mNode.setScale(Ogre::Vector3(scale));
	mNode.setDirection(aPosition.normalisedCopy(), Ogre::Node::TS_LOCAL, Ogre::Vector3::UNIT_Z);
 }

ClientTile::~ClientTile()
{
    //dtor
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

    bool pentagon = mNeighbourhood.size() == 5;

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

    assert(GetNeighbour(0));
    assert(GetNeighbour(1));
    assert(GetNeighbour(2));
    assert(GetNeighbour(3));
    assert(GetNeighbour(4));

    const Ogre::Vector3 positon = GetPosition();
    const Ogre::Vector3 normal = positon.normalisedCopy();
    const Ogre::Real len = positon.length();

    // Vetex setup
    //0
    manual.position((positon + GetNeighbour(0)->GetPosition() + GetNeighbour(1)->GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 0);
    //1
    manual.position((positon + GetNeighbour(1)->GetPosition() + GetNeighbour(2)->GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(0, 0.5);
    //2
    manual.position((positon + GetNeighbour(2)->GetPosition() + GetNeighbour(3)->GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(pentagon ? pentagonHorizont : hexagonStep, 1);
    //3
    manual.position((positon + GetNeighbour(3)->GetPosition() + GetNeighbour(4)->GetPosition()).normalisedCopy() * len);
    manual.normal(normal);
    manual.textureCoord(1.0f - (pentagon ? 0 : hexagonStep), 1.0f - (pentagon ? pentagonBottomStep : 0));

    if (pentagon)
    {
        //4
        manual.position((positon + GetNeighbour(4)->GetPosition() + GetNeighbour(0)->GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, pentagonBottomStep);
    }
    else
    {
        //4
        manual.position((positon + GetNeighbour(4)->GetPosition() + GetNeighbour(5)->GetPosition()).normalisedCopy() * len);
        manual.normal(normal);
        manual.textureCoord(1.0f, 0.5f);
        //5
        manual.position((positon + GetNeighbour(5)->GetPosition() + GetNeighbour(0)->GetPosition()).normalisedCopy() * len);
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

bool CompareEdgesAltitude(ClientTile* a, ClientTile* b)
{
    return a->GetPosition().z < b->GetPosition().z;
};


void ClientTile::SortNeighbourhood()
{
    assert(mNeighbourhood[0]);
    std::sort(mNeighbourhood.begin(), mNeighbourhood.end(), CompareEdgesAltitude);
    std::sort(mNeighbourhood.begin() + 1, mNeighbourhood.end(), CompareEdgesAngles<ClientTile>(GetPosition(), mNeighbourhood[0]->GetPosition()));
}


void ClientTile::RemoveNeighbour(ClientTile* aTile)
{
    assert(aTile);
    std::vector< ClientTile* >::iterator i = std::find(mNeighbourhood.begin(), mNeighbourhood.end(), aTile);
    assert(i != mNeighbourhood.end());
    mNeighbourhood.erase(i);
}

Ogre::Real CalcDistance(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    return acos(a.dotProduct(b));
}

ClientTile* ClientTile::GetTileAtPosition(const Ogre::Vector3& aPosistion)
{
    Ogre::Real min = CalcDistance(mNeighbourhood[0]->GetPosition(), GetPosition());
    ClientTile* currentTile = this;
    int counter = 0;

    while (CalcDistance(currentTile->GetPosition(), aPosistion) > min && counter < 1000)
    {
        ClientTile* bestNeighbour = currentTile->GetNeighbour(0);
        Ogre::Real bestDistance = CalcDistance(bestNeighbour->GetPosition(), aPosistion);
        for (size_t i = 1; i < currentTile->GetNeighbourCount(); ++i)
        {
            ClientTile* neighbour = currentTile->GetNeighbour(i);
            Ogre::Real distance = CalcDistance(neighbour->GetPosition(), aPosistion);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestNeighbour = neighbour;
            }
        }
        currentTile = bestNeighbour;
        ++counter;
    }

    return currentTile;
}
