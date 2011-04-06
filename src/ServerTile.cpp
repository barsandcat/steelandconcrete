#include <pch.h>

#include <ServerTile.h>

#include <CompareEdgesAngles.h>
#include <ServerUnit.h>

ServerTile::ServerTile(const Ogre::Vector3& aPosition, int32 aHeight):
        mPosition(aPosition),
		mHeight(aHeight),
        mTileId(0),
		mUnit(NULL)
{
    mNeighbourhood.reserve(6);
}

ServerTile::~ServerTile()
{
    //dtor
}


const Ogre::Real pentagonHorizont = 0.5f / tan(Ogre::Math::DegreesToRadians(108.0f / 2.0f));
const Ogre::Real pentagonBottomStep = (1.0f - 0.5f / sin(Ogre::Math::DegreesToRadians(108.0f / 2.0f))) / 2.0f;
const Ogre::Real hexagonStep = 0.5f / tan(Ogre::Math::DegreesToRadians(120.0f / 2.0f));

bool CompareEdgesAltitude(ServerTile* a,ServerTile* b)
{
    return a->GetPosition().z < b->GetPosition().z;
};

void ServerTile::SortNeighbourhood()
{
    assert(mNeighbourhood[0]);
    std::sort(mNeighbourhood.begin(), mNeighbourhood.end(), CompareEdgesAltitude);
    std::sort(mNeighbourhood.begin() + 1, mNeighbourhood.end(), CompareEdgesAngles(mPosition, mNeighbourhood[0]->mPosition));
}


void ServerTile::RemoveNeighbour(ServerTile& aTile)
{
    std::vector< ServerTile* >::iterator i = std::find(mNeighbourhood.begin(), mNeighbourhood.end(), &aTile);
    assert(i != mNeighbourhood.end());
    mNeighbourhood.erase(i);
}

Ogre::Real CalcDistance(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    return acos(a.dotProduct(b));
}
