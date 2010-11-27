#ifndef SERVEREDGE_H
#define SERVEREDGE_H

class ServerTile;

class ServerEdge
{
public:
    ServerEdge(ServerTile& aTileA, ServerTile& aTileB);
    ~ServerEdge();
    ServerTile& GetTileA() const { return mTileA; }
    ServerTile& GetTileB() const { return mTileB; }
protected:
private:
    ServerTile& mTileA;
    ServerTile& mTileB;
};

#endif // SERVEREDGE_H
