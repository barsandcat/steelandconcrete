#ifndef EDGE_H
#define EDGE_H

#include <Ogre.h>

class ClientGridNode;

class ClientEdge
{
public:
    ClientEdge(ClientGridNode* const aTileA, ClientGridNode* const aTileB);
    ~ClientEdge();
    ClientGridNode* GetTileA() const { return mTileA; }
    ClientGridNode* GetTileB() const { return mTileB; }
protected:
private:
    ClientGridNode* const mTileA;
    ClientGridNode* const mTileB;
};

#endif // EDGE_H
