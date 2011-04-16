#ifndef GEODESICGRID_H
#define GEODESICGRID_H

#include <Edge.h>
#include <GeodesicGrid.pb.h>
#include <Network.h>

template <typename T>
class GeodesicGrid: public boost::noncopyable
{
public:
    typedef std::vector<T*> Tiles;
    GeodesicGrid(Tiles& aTiles, int32 aSize);
    GeodesicGrid(Tiles& aTiles, Network& aNetwork);
    void Send(Network& aNetwokr) const;

    Ogre::Real GetTileRadius() const;
    ~GeodesicGrid();
private:
    Tiles& mTiles;
    std::vector< Edge<T>* > mEdges;
    void Subdivide(const Ogre::Real aSphereRadius);
    void InitTiles();
};



const int32 SEA_LEVEL_MAX = 10000;

template <typename T>
GeodesicGrid<T>::GeodesicGrid(Tiles& aTiles, int aSize): mTiles(aTiles)
{
    // 2    600
    // 3   2000
    // 4  10000
    // 5  40000
    // 6 160000
    const Ogre::Real tileArea = 2.598076211 * 70; // results in approximate radius of a tile 10.3
    int tileCount = (int)(5.0f * pow(2.0f, 2 * aSize + 3)) + 2;
    const Ogre::Real sphereArea = tileArea * tileCount;
    const Ogre::Real sphereRadius = sqrt(sphereArea / (4 * Ogre::Math::PI));

    const Ogre::Real phi = 1.618033989;

    int edgeCount = tileCount * 3;

    mTiles.reserve(tileCount);
    mEdges.reserve(edgeCount);

    // Vertices of icoshaedron

    mTiles.push_back(new T(Ogre::Vector3(0.0f, 1.0f, phi).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, 1.0f, -phi).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, -1.0f, phi).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, -1.0f, -phi).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));

    mTiles.push_back(new T(Ogre::Vector3(1.0f, phi, 0.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(-1.0f, phi, 0.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(-1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));

    mTiles.push_back(new T(Ogre::Vector3(phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(-phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));
    mTiles.push_back(new T(Ogre::Vector3(-phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius, rand() % SEA_LEVEL_MAX));

    // Link icoshaedron

    //    0   0   0   0   0
    //  4 - 8 - 2 -10 - 6 - 4
    //    9 - 5 - 7 -11 - 1 - 9
    //      3   3   3   3   3

    mEdges.push_back(new Edge<T>(*mTiles[0], *mTiles[2]));
    mEdges.push_back(new Edge<T>(*mTiles[0], *mTiles[4]));
    mEdges.push_back(new Edge<T>(*mTiles[0], *mTiles[6]));
    mEdges.push_back(new Edge<T>(*mTiles[0], *mTiles[8]));
    mEdges.push_back(new Edge<T>(*mTiles[0], *mTiles[10]));

    mEdges.push_back(new Edge<T>(*mTiles[1], *mTiles[3]));
    mEdges.push_back(new Edge<T>(*mTiles[1], *mTiles[4]));
    mEdges.push_back(new Edge<T>(*mTiles[1], *mTiles[6]));
    mEdges.push_back(new Edge<T>(*mTiles[1], *mTiles[9]));
    mEdges.push_back(new Edge<T>(*mTiles[1], *mTiles[11]));

    mEdges.push_back(new Edge<T>(*mTiles[2], *mTiles[5]));
    mEdges.push_back(new Edge<T>(*mTiles[2], *mTiles[7]));
    mEdges.push_back(new Edge<T>(*mTiles[2], *mTiles[8]));
    mEdges.push_back(new Edge<T>(*mTiles[2], *mTiles[10]));

    mEdges.push_back(new Edge<T>(*mTiles[3], *mTiles[5]));
    mEdges.push_back(new Edge<T>(*mTiles[3], *mTiles[7]));
    mEdges.push_back(new Edge<T>(*mTiles[3], *mTiles[9]));
    mEdges.push_back(new Edge<T>(*mTiles[3], *mTiles[11]));

    mEdges.push_back(new Edge<T>(*mTiles[4], *mTiles[6]));
    mEdges.push_back(new Edge<T>(*mTiles[4], *mTiles[8]));
    mEdges.push_back(new Edge<T>(*mTiles[4], *mTiles[9]));

    mEdges.push_back(new Edge<T>(*mTiles[5], *mTiles[7]));
    mEdges.push_back(new Edge<T>(*mTiles[5], *mTiles[8]));
    mEdges.push_back(new Edge<T>(*mTiles[5], *mTiles[9]));

    mEdges.push_back(new Edge<T>(*mTiles[6], *mTiles[10]));
    mEdges.push_back(new Edge<T>(*mTiles[6], *mTiles[11]));

    mEdges.push_back(new Edge<T>(*mTiles[7], *mTiles[10]));
    mEdges.push_back(new Edge<T>(*mTiles[7], *mTiles[11]));

    mEdges.push_back(new Edge<T>(*mTiles[8], *mTiles[9]));

    mEdges.push_back(new Edge<T>(*mTiles[10], *mTiles[11]));

    for (int i = 0; i <= aSize; ++i)
    {
        Subdivide(sphereRadius);
    }

    InitTiles();
}

template <typename T>
GeodesicGrid<T>::~GeodesicGrid()
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

template <typename T>
void GeodesicGrid<T>::Subdivide(const Ogre::Real aSphereRadius)
{
    Tiles newTiles;
    newTiles.reserve(mEdges.size());
    std::vector< Edge<T>* > newEdges;

    // Dividing edges
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        Edge<T>* edge = mEdges[i];
        const Ogre::Vector3& a = edge->GetTileA().GetPosition();
        const Ogre::Vector3& b = edge->GetTileB().GetPosition();

        int32 err = a.distance(b) * SEA_LEVEL_MAX;
        float rnd = (rand() % 100 + 1) / 100.0f - 0.5f;
        int32 height = (edge->GetTileA().GetHeight() + edge->GetTileB().GetHeight()) / 2 + rnd * err;

        T* tile = new T((a + b).normalisedCopy() * aSphereRadius, height);
        newEdges.push_back(new Edge<T>(*tile, edge->GetTileA()));
        newEdges.push_back(new Edge<T>(*tile, edge->GetTileB()));
        newTiles.push_back(tile);
    }

    // Replacing edges
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        mEdges[i]->Unlink();
        delete mEdges[i];
    }
    mEdges = newEdges;


    // Linking new tiles (mTiles holds only old tiles)
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        T* tile = mTiles[i];
        tile->SortNeighbourhood();
        for (size_t i = 0; i < tile->GetNeighbourCount(); ++i)
        {
            size_t to = i + 1;
            if (to >= tile->GetNeighbourCount())
            {
                to = 0;
            }
            mEdges.push_back(new Edge<T>(tile->GetNeighbour(i), tile->GetNeighbour(to)));
        }
    }

    // Appending tiles
    mTiles.insert(mTiles.end(), newTiles.begin(), newTiles.end());

}


template <typename T>
void GeodesicGrid<T>::InitTiles()
{
    for (TileId i = 0; i < mTiles.size(); ++i)
    {
        mTiles[i]->SortNeighbourhood();
        mTiles[i]->SetTileId(i);
    }
}

template <typename T>
Ogre::Real GeodesicGrid<T>::GetTileRadius() const
{
    Ogre::Real sum = 0;
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        const Edge<T>* edge = mEdges.at(i);
        sum += (edge->GetTileA().GetPosition() - edge->GetTileB().GetPosition()).squaredLength();
    }
    return sqrt(sum / mEdges.size() / 2.0f);
}

template <typename T>
void GeodesicGrid<T>::Send(Network& aNetwork) const
{
    GeodesicGridSizeMsg gridInfo;
    gridInfo.set_tilecount(mTiles.size());
    gridInfo.set_edgecount(mEdges.size());
    aNetwork.WriteMessage(gridInfo);
    //GetLog() << "Grid info send " << gridInfo.ShortDebugString();
    const size_t tilesPerMessage = 100;
    for (size_t i = 0; i < mTiles.size();)
    {
        TileListMsg tiles;
        for (size_t j = 0; j < tilesPerMessage && i < mTiles.size(); ++j)
        {
            TileMsg* tile = tiles.add_tiles();
            Ogre::Vector3 pos = mTiles[i]->GetPosition();
            tile->set_tag(i);
            tile->set_height(mTiles[i]->GetHeight());
            tile->mutable_position()->set_x(pos.x);
            tile->mutable_position()->set_y(pos.y);
            tile->mutable_position()->set_z(pos.z);
            ++i;
        }
        aNetwork.WriteMessage(tiles);
    }

    //GetLog() << "Send all tiles";

    const size_t edgesPerMessage = 100;
    for (size_t i = 0; i < mEdges.size();)
    {
        EdgeListMsg edges;
        for (size_t j = 0; j < edgesPerMessage && i < mEdges.size(); ++j)
        {
            EdgeMsg* edge = edges.add_edges();
            edge->set_tilea(mEdges[i]->GetTileA().GetTileId());
            edge->set_tileb(mEdges[i]->GetTileB().GetTileId());
            ++i;
        }
        aNetwork.WriteMessage(edges);
    }
    //GetLog() << "Send all edges";
}

template <typename T>
GeodesicGrid<T>::GeodesicGrid(Tiles& aTiles, Network& aNetwork): mTiles(aTiles)
{
    GeodesicGridSizeMsg gridInfo;
    aNetwork.ReadMessage(gridInfo);
    //GetLog() << "Recived grid info " << gridInfo.ShortDebugString();
    mTiles.resize(gridInfo.tilecount());
    mEdges.resize(gridInfo.edgecount());

    for (size_t i = 0; i < gridInfo.tilecount();)
    {
        TileListMsg tiles;
        aNetwork.ReadMessage(tiles);
        for (int j = 0; j < tiles.tiles_size(); ++j)
        {
            TileMsg tile = tiles.tiles(j);
            Ogre::Vector3 pos(tile.position().x(), tile.position().y(), tile.position().z());
            T* node = new T(tile.tag(), pos);
            mTiles[tile.tag()] = node;

            ++i;
        }
    }
    //GetLog() << "Recived all tiles";

    for (size_t i = 0; i < gridInfo.edgecount();)
    {
        EdgeListMsg edges;
        aNetwork.ReadMessage(edges);
        for (int j = 0; j < edges.edges_size(); ++j)
        {
            EdgeMsg edge = edges.edges(j);
            mEdges[i] = new Edge<T>(*mTiles[edge.tilea()], *mTiles[edge.tileb()]);
            ++i;
        }
    }
    //GetLog() << "Recived all edges ";

    InitTiles();
}




#endif // SERVERGEODESICGRID_H

