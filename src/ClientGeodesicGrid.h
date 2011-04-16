#ifndef CLIENTGEODESICGRID_H
#define CLIENTGEODESICGRID_H

#include <ClientGridNode.h>
#include <ClientEdge.h>
#include <LoadingSheet.h>
#include <Typedefs.h>
#include <Network.h>

class ClientGeodesicGrid
{
public:
    ClientGeodesicGrid(Network& aNetwork);

    ClientGridNode& GetGridNode(TileId aIndex) const { return *mTiles[aIndex]; }
    ClientEdge& GetEdge(size_t aIndex) const { return *mEdges[aIndex]; }
    TileId GetTileCount() const { return mTiles.size(); }
    size_t GetEdgeCount() const { return mEdges.size(); }
    ~ClientGeodesicGrid();
protected:
private:
    std::vector< ClientGridNode* > mTiles;
    std::vector< ClientEdge* > mEdges;
    void InitTiles();
};

#endif // CLIENTGEODESICGRID_H
