#ifndef SERVERGEODESICGRID_H
#define SERVERGEODESICGRID_H

#include <ServerTile.h>
#include <ServerEdge.h>
#include <Network.h>

class ServerGeodesicGrid
{
public:
    ServerGeodesicGrid(int32 aSize, int32 aSeaLevel);
    ServerGeodesicGrid(const Ogre::String aFileName);

    void Save(const Ogre::String aFileName) const;
    void Send(Network& aNetwokr) const;
    ServerTile& GetTile(size_t aIndex) const { return *mTiles[aIndex]; }
    ServerEdge& GetEdge(size_t aIndex) const { return *mEdges[aIndex]; }
    size_t GetTileCount() const { return mTiles.size(); }
    size_t GetEdgeCount() const { return mEdges.size(); }
    Ogre::Real GetTileRadius() const;
    ~ServerGeodesicGrid();
protected:
private:
    std::vector< ServerTile* > mTiles;
    std::vector< ServerEdge* > mEdges;
    int32 mSeaLevel;
    const int32 mSize;
    void Subdivide(const Ogre::Real aSphereRadius);
    void InitTiles();
};

#endif // SERVERGEODESICGRID_H
