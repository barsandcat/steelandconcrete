#ifndef EDGE_H
#define EDGE_H

#include <Ogre.h>

class ClientTile;

class ClientEdge
{
public:
    ClientEdge(ClientTile* const aTileA, ClientTile* const aTileB);
    ~ClientEdge();
    ClientTile* GetTileA() const { return mTileA; }
    ClientTile* GetTileB() const { return mTileB; }
protected:
private:
    ClientTile* const mTileA;
    ClientTile* const mTileB;
};

#endif // EDGE_H
