#include <pch.h>

#include <ClientGridNode.h>
#include <CompareEdgesAngles.h>
#include <ClientApp.h>
#include <ClientTile.h>

ClientGridNode::ClientGridNode(TileId aId, const Ogre::Vector3& aPosition):
        mPosition(aPosition),
        mTileId(aId),
        mTile(NULL)
{
    mNeighbourhood.reserve(6);
}

void ClientGridNode::CreateTile(bool ground)
{
    mTile = new ClientTile(ground, *this);
}


ClientGridNode::~ClientGridNode()
{
    delete mTile;    //dtor
}

bool CompareEdgesAltitude(ClientGridNode* a, ClientGridNode* b)
{
    return a->GetPosition().z < b->GetPosition().z;
};


void ClientGridNode::SortNeighbourhood()
{
    assert(mNeighbourhood[0]);
    std::sort(mNeighbourhood.begin(), mNeighbourhood.end(), CompareEdgesAltitude);
    std::sort(mNeighbourhood.begin() + 1, mNeighbourhood.end(), CompareEdgesAngles<ClientGridNode>(GetPosition(), mNeighbourhood[0]->GetPosition()));
}


void ClientGridNode::RemoveNeighbour(ClientGridNode& aTile)
{
    std::vector< ClientGridNode* >::iterator i = std::find(mNeighbourhood.begin(), mNeighbourhood.end(), &aTile);
    assert(i != mNeighbourhood.end());
    mNeighbourhood.erase(i);
}

Ogre::Real CalcDistance2(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    return acos(a.dotProduct(b));
}

ClientGridNode* ClientGridNode::GetTileAtPosition(const Ogre::Vector3& aPosistion)
{
    Ogre::Real min = CalcDistance2(mNeighbourhood[0]->GetPosition(), GetPosition());
    ClientGridNode* currentTile = this;
    int counter = 0;

    while (CalcDistance2(currentTile->GetPosition(), aPosistion) > min && counter < 1000)
    {
        ClientGridNode* bestNeighbour = currentTile->GetNeighbour(0);
        Ogre::Real bestDistance = CalcDistance2(bestNeighbour->GetPosition(), aPosistion);
        for (size_t i = 1; i < currentTile->GetNeighbourCount(); ++i)
        {
            ClientGridNode* neighbour = currentTile->GetNeighbour(i);
            Ogre::Real distance = CalcDistance2(neighbour->GetPosition(), aPosistion);
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
