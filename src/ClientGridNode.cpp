#include <pch.h>

#include <ClientGridNode.h>
#include <CompareEdgesAngles.h>
#include <ClientApp.h>
#include <ClientTile.h>

ClientGridNode::ClientGridNode(const Ogre::Vector3& aPosition):
        mPosition(aPosition),
        mTile(NULL)
{
    mNeighbourhood.reserve(6);
}

void ClientGridNode::CreateTile(bool ground)
{
    if (!mTile)
    {
        mTile = new ClientTile(ground, *this);
    }
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

ClientGridNode* ClientGridNode::GetTileAtPosition(const Ogre::Vector3& aPosistion)
{
    Ogre::Radian min = CalcDistance2(mNeighbourhood[0]->GetPosition(), GetPosition());
    ClientGridNode* currentTile = this;
    int counter = 0;

    while (CalcDistance2(currentTile->GetPosition(), aPosistion) > min && counter < 1000)
    {
        ClientGridNode* bestNeighbour = &currentTile->GetNeighbour(0);
        Ogre::Radian bestDistance = CalcDistance2(bestNeighbour->GetPosition(), aPosistion);
        for (size_t i = 1; i < currentTile->GetNeighbourCount(); ++i)
        {
            ClientGridNode* neighbour = &currentTile->GetNeighbour(i);
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
