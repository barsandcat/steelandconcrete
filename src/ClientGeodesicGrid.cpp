#include <pch.h>
#include <ClientGeodesicGrid.h>
#include <GeodesicGrid.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <ClientApp.h>
#include <ClientTile.h>

ClientGeodesicGrid::~ClientGeodesicGrid()
{
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        delete mEdges[i];
        mEdges[i] = NULL;
    }
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        delete mTiles[i];
        mTiles[i] = NULL;
    }
}

void ClientGeodesicGrid::InitTiles()
{
    for (TileId i = 0; i < mTiles.size(); ++i)
    {
        mTiles[i]->SortNeighbourhood();
    }
}


ClientGeodesicGrid::ClientGeodesicGrid(Network& aNetwork)
{
    GeodesicGridSizeMsg gridInfo;
    aNetwork.ReadMessage(gridInfo);
    GetLog() << "Recived grid info " << gridInfo.ShortDebugString();
    mTiles.resize(gridInfo.tilecount());
    mEdges.resize(gridInfo.edgecount());
    int32 seaLevel = gridInfo.sealevel();

    for (size_t i = 0; i < gridInfo.tilecount();)
    {
        TileListMsg tiles;
        aNetwork.ReadMessage(tiles);
        for (int j = 0; j < tiles.tiles_size(); ++j)
        {
            TileMsg tile = tiles.tiles(j);
            Ogre::Vector3 pos(tile.position().x(), tile.position().y(), tile.position().z());
            ClientGridNode* node = new ClientGridNode(tile.tag(), pos);
            mTiles[tile.tag()] = node;

            ++i;
        }
    }
    GetLog() << "Recived all tiles";

    for (size_t i = 0; i < gridInfo.edgecount();)
    {
        EdgeListMsg edges;
        aNetwork.ReadMessage(edges);
        for (int j = 0; j < edges.edges_size(); ++j)
        {
            EdgeMsg edge = edges.edges(j);
            mEdges[i] = new ClientEdge(*mTiles[edge.tilea()], *mTiles[edge.tileb()]);
            ++i;
        }
    }
    GetLog() << "Recived all edges ";

    InitTiles();
}
