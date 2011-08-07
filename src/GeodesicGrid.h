#ifndef GEODESICGRID_H
#define GEODESICGRID_H

#include <Edge.h>
#include <Network.h>

template <typename T>
class GeodesicGrid: public boost::noncopyable
{
public:
    typedef std::vector<T*> Tiles;
    GeodesicGrid(Tiles& aTiles, int32 aSize);

    Ogre::Real GetTileRadius() const;
    ~GeodesicGrid();
private:
    std::vector< Edge<T>* > mEdges;
    void Subdivide(const Ogre::Real aSphereRadius, Tiles& aTiles);
    void InitTiles(Tiles& aTiles);
    TileId mIdCounter;
};

template <typename T>
GeodesicGrid<T>::GeodesicGrid(Tiles& aTiles, int aSize)
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

    aTiles.reserve(tileCount);
    mEdges.reserve(edgeCount);

    // Vertices of icoshaedron

    aTiles.push_back(new T( 0, Ogre::Vector3(0.0f, 1.0f, phi).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 1, Ogre::Vector3(0.0f, 1.0f, -phi).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 2, Ogre::Vector3(0.0f, -1.0f, phi).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 3, Ogre::Vector3(0.0f, -1.0f, -phi).normalisedCopy() * sphereRadius));

    aTiles.push_back(new T( 4, Ogre::Vector3(1.0f, phi, 0.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 5, Ogre::Vector3(1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 6, Ogre::Vector3(-1.0f, phi, 0.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 7, Ogre::Vector3(-1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius));

    aTiles.push_back(new T( 8, Ogre::Vector3(phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T( 9, Ogre::Vector3(phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T(10, Ogre::Vector3(-phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius));
    aTiles.push_back(new T(11, Ogre::Vector3(-phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius));
    mIdCounter = 12;

    // Link icoshaedron

    //    0   0   0   0   0
    //  4 - 8 - 2 -10 - 6 - 4
    //    9 - 5 - 7 -11 - 1 - 9
    //      3   3   3   3   3

    mEdges.push_back(new Edge<T>(*aTiles[0], *aTiles[2]));
    mEdges.push_back(new Edge<T>(*aTiles[0], *aTiles[4]));
    mEdges.push_back(new Edge<T>(*aTiles[0], *aTiles[6]));
    mEdges.push_back(new Edge<T>(*aTiles[0], *aTiles[8]));
    mEdges.push_back(new Edge<T>(*aTiles[0], *aTiles[10]));

    mEdges.push_back(new Edge<T>(*aTiles[1], *aTiles[3]));
    mEdges.push_back(new Edge<T>(*aTiles[1], *aTiles[4]));
    mEdges.push_back(new Edge<T>(*aTiles[1], *aTiles[6]));
    mEdges.push_back(new Edge<T>(*aTiles[1], *aTiles[9]));
    mEdges.push_back(new Edge<T>(*aTiles[1], *aTiles[11]));

    mEdges.push_back(new Edge<T>(*aTiles[2], *aTiles[5]));
    mEdges.push_back(new Edge<T>(*aTiles[2], *aTiles[7]));
    mEdges.push_back(new Edge<T>(*aTiles[2], *aTiles[8]));
    mEdges.push_back(new Edge<T>(*aTiles[2], *aTiles[10]));

    mEdges.push_back(new Edge<T>(*aTiles[3], *aTiles[5]));
    mEdges.push_back(new Edge<T>(*aTiles[3], *aTiles[7]));
    mEdges.push_back(new Edge<T>(*aTiles[3], *aTiles[9]));
    mEdges.push_back(new Edge<T>(*aTiles[3], *aTiles[11]));

    mEdges.push_back(new Edge<T>(*aTiles[4], *aTiles[6]));
    mEdges.push_back(new Edge<T>(*aTiles[4], *aTiles[8]));
    mEdges.push_back(new Edge<T>(*aTiles[4], *aTiles[9]));

    mEdges.push_back(new Edge<T>(*aTiles[5], *aTiles[7]));
    mEdges.push_back(new Edge<T>(*aTiles[5], *aTiles[8]));
    mEdges.push_back(new Edge<T>(*aTiles[5], *aTiles[9]));

    mEdges.push_back(new Edge<T>(*aTiles[6], *aTiles[10]));
    mEdges.push_back(new Edge<T>(*aTiles[6], *aTiles[11]));

    mEdges.push_back(new Edge<T>(*aTiles[7], *aTiles[10]));
    mEdges.push_back(new Edge<T>(*aTiles[7], *aTiles[11]));

    mEdges.push_back(new Edge<T>(*aTiles[8], *aTiles[9]));

    mEdges.push_back(new Edge<T>(*aTiles[10], *aTiles[11]));

    for (int i = 0; i <= aSize; ++i)
    {
        Subdivide(sphereRadius, aTiles);
    }

    InitTiles(aTiles);
}

template <typename T>
GeodesicGrid<T>::~GeodesicGrid()
{
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        delete mEdges[i];
        mEdges[i] = NULL;
    }
}

template <typename T>
void GeodesicGrid<T>::Subdivide(const Ogre::Real aSphereRadius, Tiles& aTiles)
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

        T* tile = new T(mIdCounter, (a + b).normalisedCopy() * aSphereRadius);
        mIdCounter += 1;

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
    for (size_t i = 0; i < aTiles.size(); ++i)
    {
        T* tile = aTiles[i];
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
    aTiles.insert(aTiles.end(), newTiles.begin(), newTiles.end());

}


template <typename T>
void GeodesicGrid<T>::InitTiles(Tiles& aTiles)
{
    for (TileId i = 0; i < aTiles.size(); ++i)
    {
        aTiles[i]->SortNeighbourhood();
        assert(aTiles[i]->GetTileId() == i);
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


#endif // SERVERGEODESICGRID_H

