#ifndef CLIENTGEODESICGRID_H
#define CLIENTGEODESICGRID_H

#include <ClientTile.h>
#include <ClientEdge.h>
#include <LoadingSheet.h>
#include <sockio.h>

class ClientGeodesicGrid
{
public:
    ClientGeodesicGrid(socket_t& aSocket, LoadingSheet& loadingSheet);

    Ogre::StaticGeometry* ConstructStaticGeometry(Ogre::SceneManager& aSceneManager) const;
    Ogre::ManualObject* ConstructDebugMesh() const;
    ClientTile& GetTile(size_t aIndex) const { return *mTiles[aIndex]; }
    ClientEdge& GetEdge(size_t aIndex) const { return *mEdges[aIndex]; }
    size_t GetTileCount() const { return mTiles.size(); }
    size_t GetEdgeCount() const { return mEdges.size(); }
    ~ClientGeodesicGrid();
protected:
private:
    std::vector< ClientTile* > mTiles;
    std::vector< ClientEdge* > mEdges;
    void InitTiles();
};

#endif // CLIENTGEODESICGRID_H
