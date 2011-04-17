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

    Ogre::Real GetTileRadius() const;
    ~GeodesicGrid();
private:
    Tiles& mTiles;
    std::vector< Edge<T>* > mEdges;
    void Subdivide(const Ogre::Real aSphereRadius);
    void InitTiles();
};

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

    mTiles.push_back(new T(Ogre::Vector3(0.0f, 1.0f, phi).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, 1.0f, -phi).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, -1.0f, phi).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(0.0f, -1.0f, -phi).normalisedCopy() * sphereRadius));

    mTiles.push_back(new T(Ogre::Vector3(1.0f, phi, 0.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(-1.0f, phi, 0.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(-1.0f, -phi, 0.0f).normalisedCopy() * sphereRadius));

    mTiles.push_back(new T(Ogre::Vector3(phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(-phi, 0.0f, 1.0f).normalisedCopy() * sphereRadius));
    mTiles.push_back(new T(Ogre::Vector3(-phi, 0.0f, -1.0f).normalisedCopy() * sphereRadius));

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

        T* tile = new T((a + b).normalisedCopy() * aSphereRadius);
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


#endif // SERVERGEODESICGRID_H

