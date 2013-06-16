#include <pch.h>

#include <ClientTile.h>
#include <CompareEdgesAngles.h>
#include <ClientApp.h>
#include <TileEntity.h>

ClientTile::ClientTile(TileId aId, const Ogre::Vector3& aPosition):
        mPosition(aPosition),
        mTile(NULL),
        mUnit(NULL),
        mTileId(aId)
{
    mNeighbourhood.reserve(6);
}

void ClientTile::CreateEntity(bool ground)
{
    if (!mTile)
    {
        mTile = new TileEntity(ground, *this);
    }
}

void ClientTile::DestroyEntity()
{
    delete mTile;
    mTile = NULL;
}

ClientTile::~ClientTile()
{
    delete mTile;
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


void ClientTile::RemoveNeighbour(ClientTile& aTile)
{
    std::vector< ClientTile* >::iterator i = std::find(mNeighbourhood.begin(), mNeighbourhood.end(), &aTile);
    assert(i != mNeighbourhood.end());
    mNeighbourhood.erase(i);
}

Ogre::Radian CalcDistance2(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    Ogre::Real lenProduct = a.length() * b.length();

    // Divide by zero check
    if(lenProduct < 1e-6f)
        lenProduct = 1e-6f;

    Ogre::Real f = a.dotProduct(b) / lenProduct;

    f = Ogre::Math::Clamp(f, (Ogre::Real)-1.0, (Ogre::Real)1.0);
    return Ogre::Math::ACos(f);
}

ClientTile* ClientTile::GetTileAtPosition(const Ogre::Vector3& aPosistion)
{
    Ogre::Radian min = CalcDistance2(mNeighbourhood[0]->GetPosition(), GetPosition());
    ClientTile* currentTile = this;
    int counter = 0;

    while (CalcDistance2(currentTile->GetPosition(), aPosistion) > min && counter < 1000)
    {
        ClientTile* bestNeighbour = &currentTile->GetNeighbour(0);
        Ogre::Radian bestDistance = CalcDistance2(bestNeighbour->GetPosition(), aPosistion);
        for (size_t i = 1; i < currentTile->GetNeighbourCount(); ++i)
        {
            ClientTile* neighbour = &currentTile->GetNeighbour(i);
            Ogre::Radian distance = CalcDistance2(neighbour->GetPosition(), aPosistion);
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
