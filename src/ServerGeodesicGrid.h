#ifndef SERVERGEODESICGRID_H
#define SERVERGEODESICGRID_H

#include <ServerTile.h>
#include <ServerEdge.h>
#include <sockio.h>

class ServerGeodesicGrid
{
public:
    ServerGeodesicGrid(int aSize);
    ServerGeodesicGrid(const Ogre::String aFileName);

    void Save(const Ogre::String aFileName) const;
    void Send(socket_t& aSocket) const;
    ServerTile& GetTile(size_t aIndex) const { return *mTiles[aIndex]; }
    ServerEdge& GetEdge(size_t aIndex) const { return *mEdges[aIndex]; }
    size_t GetTileCount() const { return mTiles.size(); }
    size_t GetEdgeCount() const { return mEdges.size(); }
    ~ServerGeodesicGrid();
protected:
private:
    std::vector< ServerTile* > mTiles;
    std::vector< ServerEdge* > mEdges;
    void Subdivide();
    void InitTiles();
};

#endif // SERVERGEODESICGRID_H
