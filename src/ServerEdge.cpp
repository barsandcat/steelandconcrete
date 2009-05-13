#include <ServerEdge.h>
#include <ServerTile.h>


ServerEdge::ServerEdge(ServerTile& aTileA, ServerTile& aTileB): mTileA(aTileA), mTileB(aTileB)
{
    mTileA.AddNeighbour(mTileB);
    mTileB.AddNeighbour(mTileA);
}

ServerEdge::~ServerEdge()
{
    mTileA.RemoveNeighbour(mTileB);
    mTileB.RemoveNeighbour(mTileA);
}

