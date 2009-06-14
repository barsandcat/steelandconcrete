#include <pch.h>
#include <ServerGeodesicGrid.h>

#include <GeodesicGrid.pb.h>
#include <Network.h>
#include <ServerLog.h>

ServerGeodesicGrid::ServerGeodesicGrid(int aSize)
{
    // 2    600
    // 3   2000
    // 4  10000
    // 5  40000
    // 6 160000

    const Ogre::Real phi = (1.0f + sqrt(5.0f)) / 2.0f;
    int tileCount = (int)(5.0f * pow(2.0f, 2 * aSize + 3)) + 2;
    int edgeCount = tileCount * 6 / 2;

    mTiles.reserve(tileCount);
    mEdges.reserve(edgeCount);

    // Vertices of icoshaedron

    mTiles.push_back(new ServerTile(Ogre::Vector3(0.0f, 1.0f, phi).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(0.0f, 1.0f, -phi).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(0.0f, -1.0f, phi).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(0.0f, -1.0f, -phi).normalisedCopy()));

    mTiles.push_back(new ServerTile(Ogre::Vector3(1.0f, phi, 0.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(1.0f, -phi, 0.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(-1.0f, phi, 0.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(-1.0f, -phi, 0.0f).normalisedCopy()));

    mTiles.push_back(new ServerTile(Ogre::Vector3(phi, 0.0f, 1.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(phi, 0.0f, -1.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(-phi, 0.0f, 1.0f).normalisedCopy()));
    mTiles.push_back(new ServerTile(Ogre::Vector3(-phi, 0.0f, -1.0f).normalisedCopy()));

    // Link icoshaedron

    //    0   0   0   0   0
    //  4 - 8 - 2 -10 - 6 - 4
    //    9 - 5 - 7 -11 - 1 - 9
    //      3   3   3   3   3

    mEdges.push_back(new ServerEdge(*mTiles[0], *mTiles[2]));
    mEdges.push_back(new ServerEdge(*mTiles[0], *mTiles[4]));
    mEdges.push_back(new ServerEdge(*mTiles[0], *mTiles[6]));
    mEdges.push_back(new ServerEdge(*mTiles[0], *mTiles[8]));
    mEdges.push_back(new ServerEdge(*mTiles[0], *mTiles[10]));

    mEdges.push_back(new ServerEdge(*mTiles[1], *mTiles[3]));
    mEdges.push_back(new ServerEdge(*mTiles[1], *mTiles[4]));
    mEdges.push_back(new ServerEdge(*mTiles[1], *mTiles[6]));
    mEdges.push_back(new ServerEdge(*mTiles[1], *mTiles[9]));
    mEdges.push_back(new ServerEdge(*mTiles[1], *mTiles[11]));

    mEdges.push_back(new ServerEdge(*mTiles[2], *mTiles[5]));
    mEdges.push_back(new ServerEdge(*mTiles[2], *mTiles[7]));
    mEdges.push_back(new ServerEdge(*mTiles[2], *mTiles[8]));
    mEdges.push_back(new ServerEdge(*mTiles[2], *mTiles[10]));

    mEdges.push_back(new ServerEdge(*mTiles[3], *mTiles[5]));
    mEdges.push_back(new ServerEdge(*mTiles[3], *mTiles[7]));
    mEdges.push_back(new ServerEdge(*mTiles[3], *mTiles[9]));
    mEdges.push_back(new ServerEdge(*mTiles[3], *mTiles[11]));

    mEdges.push_back(new ServerEdge(*mTiles[4], *mTiles[6]));
    mEdges.push_back(new ServerEdge(*mTiles[4], *mTiles[8]));
    mEdges.push_back(new ServerEdge(*mTiles[4], *mTiles[9]));

    mEdges.push_back(new ServerEdge(*mTiles[5], *mTiles[7]));
    mEdges.push_back(new ServerEdge(*mTiles[5], *mTiles[8]));
    mEdges.push_back(new ServerEdge(*mTiles[5], *mTiles[9]));

    mEdges.push_back(new ServerEdge(*mTiles[6], *mTiles[10]));
    mEdges.push_back(new ServerEdge(*mTiles[6], *mTiles[11]));

    mEdges.push_back(new ServerEdge(*mTiles[7], *mTiles[10]));
    mEdges.push_back(new ServerEdge(*mTiles[7], *mTiles[11]));

    mEdges.push_back(new ServerEdge(*mTiles[8], *mTiles[9]));

    mEdges.push_back(new ServerEdge(*mTiles[10], *mTiles[11]));

    for (int i = 0; i <= aSize; ++i)
    {
        Subdivide();
    }

    InitTiles();
}

ServerGeodesicGrid::~ServerGeodesicGrid()
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

void ServerGeodesicGrid::Subdivide()
{
    std::vector< ServerTile* > newTiles;
    newTiles.reserve(mEdges.size());
    std::vector< ServerEdge* > newEdges;

    // Dividing edges
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        ServerEdge* edge = mEdges[i];
        ServerTile* tile = new ServerTile((edge->GetTileA().GetPosition() + edge->GetTileB().GetPosition()).normalisedCopy());
        newEdges.push_back(new ServerEdge(*tile, edge->GetTileA()));
        newEdges.push_back(new ServerEdge(*tile, edge->GetTileB()));
        newTiles.push_back(tile);
    }

    // Replacing edges
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        delete mEdges[i];
    }
    mEdges = newEdges;


    // Linking new tiles (mTiles holds only old tiles)
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        ServerTile* tile = mTiles[i];
        tile->SortNeighbourhood();
        for (size_t i = 0; i < tile->GetNeighbourCount(); ++i)
        {
            size_t to = i + 1;
            if (to >= tile->GetNeighbourCount())
            {
                to = 0;
            }
            mEdges.push_back(new ServerEdge(tile->GetNeighbour(i), tile->GetNeighbour(to)));
        }
    }

    // Appending tiles
    mTiles.insert(mTiles.end(), newTiles.begin(), newTiles.end());

}


void ServerGeodesicGrid::InitTiles()
{
    for (TileId i = 0; i < mTiles.size(); ++i)
    {
        mTiles[i]->SortNeighbourhood();
        mTiles[i]->SetTileId(i);
    }
}

ServerGeodesicGrid::ServerGeodesicGrid(const Ogre::String aFileName)
{
    GeodesicGridMsg grid;
    std::fstream input(aFileName.c_str(), std::ios::in | std::ios::binary);
    if (grid.ParseFromIstream(&input))
    {
        mTiles.reserve(grid.tiles_size());
        for (int i = 0; i < grid.tiles_size(); ++i)
        {
            const TileMsg& tile = grid.tiles(i);
            const VectorMsg& vector = tile.position();
            Ogre::Vector3 position(vector.x(), vector.y(), vector.z());
            ServerTile* newTile = new ServerTile(position);
            newTile->SetTileId(i);
            mTiles.push_back(newTile);
        }

        mEdges.reserve(grid.edges_size());
        for (int i = 0; i < grid.edges_size(); ++i)
        {
            const EdgeMsg& edge = grid.edges(i);
            mEdges.push_back(new ServerEdge(*mTiles[edge.tilea()], *mTiles[edge.tileb()]));
        }

        InitTiles();
    }
}

void ServerGeodesicGrid::Save(const Ogre::String aFileName) const
{
    GeodesicGridMsg grid;

    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        Ogre::Vector3 pos = mTiles[i]->GetPosition();
        TileMsg* tile = grid.add_tiles();
        tile->set_tag(i);
        tile->mutable_position()->set_x(pos.x);
        tile->mutable_position()->set_y(pos.y);
        tile->mutable_position()->set_z(pos.z);
    }

    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        EdgeMsg* edge = grid.add_edges();
        edge->set_tilea(mEdges[i]->GetTileA().GetTileId());
        edge->set_tileb(mEdges[i]->GetTileB().GetTileId());
    }

    std::fstream output(aFileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    grid.SerializeToOstream(&output);
}

void ServerGeodesicGrid::Send(socket_t& aSocket) const
{
    GeodesicGridSizeMsg gridInfo;
    gridInfo.set_tilecount(mTiles.size());
    gridInfo.set_edgecount(mEdges.size());
    gridInfo.set_scale((mTiles[0]->GetPosition() - mTiles[0]->GetNeighbour(0).GetPosition()).length());
    WriteMessage(aSocket, gridInfo);
    GetLog() << "Grid info send " << gridInfo.ShortDebugString() << std::endl;

    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        TileMsg tile;
        tile.set_tag(i);
        Ogre::Vector3 pos = mTiles[i]->GetPosition();
        tile.mutable_position()->set_x(pos.x);
        tile.mutable_position()->set_y(pos.y);
        tile.mutable_position()->set_z(pos.z);
        WriteMessage(aSocket, tile);
    }
    GetLog() << "Send all tiles" << std::endl;

    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        EdgeMsg edge;
        edge.set_tilea(mEdges[i]->GetTileA().GetTileId());
        edge.set_tileb(mEdges[i]->GetTileB().GetTileId());
        WriteMessage(aSocket, edge);
    }
    GetLog() << "Send all edges" << std::endl;
}
