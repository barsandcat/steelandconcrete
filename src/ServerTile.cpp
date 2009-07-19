#include <pch.h>
#include <ServerTile.h>

#include <ServerUnit.h>

ServerTile::ServerTile(const Ogre::Vector3& aPosition):
        mPosition(aPosition),
        mTileId(0)
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

struct CompareEdgesAngles
{
    CompareEdgesAngles(const Ogre::Vector3& aRoot, const Ogre::Vector3&  aPole): mRoot(aRoot), mPole(aPole) {}
    bool operator()(ServerTile* a, ServerTile* b)
    {
        return CalcAngle(a->GetPosition()) < CalcAngle(b->GetPosition());
    }

    Ogre::Radian CalcAngle(const Ogre::Vector3& u) const
    {
        // Two vectors that forms angle between pole and neighbour
        Ogre::Vector3 a = mRoot.crossProduct(u);
        Ogre::Vector3 b = mRoot.crossProduct(mPole);

        // Angle
        Ogre::Real C = a.angleBetween(b).valueRadians();

        // Rotation axis
        Ogre::Vector3 normal(a.crossProduct(b).normalisedCopy());

        // Correct angle, if rotation axis is not equal to our positon vector -
        if (normal != Ogre::Vector3::ZERO)
        {
            Ogre::Vector3 delta(mRoot - normal);
            if (delta.squaredLength() > 0.1)
            {
                C = Ogre::Math::TWO_PI - C;
            }
        }

        assert(C >= 0 && C <= Ogre::Math::TWO_PI);
        return Ogre::Radian(C);
    }
    const Ogre::Vector3&  mRoot;
    const Ogre::Vector3&  mPole;
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
