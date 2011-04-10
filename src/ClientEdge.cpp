#include <pch.h>
#include <ClientEdge.h>
#include <ClientGridNode.h>


ClientEdge::ClientEdge(ClientGridNode* const aTileA, ClientGridNode* const aTileB): mTileA(aTileA), mTileB(aTileB)
{
    mTileA->AddNeighbour(mTileB);
    mTileB->AddNeighbour(mTileA);
}

ClientEdge::~ClientEdge()
{
    mTileA->RemoveNeighbour(mTileB);
    mTileB->RemoveNeighbour(mTileA);
}

