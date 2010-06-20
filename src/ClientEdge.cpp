#include <pch.h>
#include <ClientEdge.h>
#include <ClientTile.h>


ClientEdge::ClientEdge(ClientTile* const aTileA, ClientTile* const aTileB): mTileA(aTileA), mTileB(aTileB)
{
    mTileA->AddNeighbour(mTileB);
    mTileB->AddNeighbour(mTileA);
}

ClientEdge::~ClientEdge()
{
    mTileA->RemoveNeighbour(mTileB);
    mTileB->RemoveNeighbour(mTileA);
}

