#ifndef CLIENTGRIDNODE_H
#define CLIENTGRIDNODE_H


class ClientUnit;
class ClientTile;

class ClientGridNode: public boost::noncopyable
{
public:
    explicit ClientGridNode(TileId aId, const Ogre::Vector3& aPosition);
    ~ClientGridNode();
    void CreateTile(bool ground);
    void AddNeighbour(ClientGridNode& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ClientGridNode& aTile);
    void SortNeighbourhood();

    Ogre::Vector3 GetPosition() const { return mPosition; }
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientGridNode* GetNeighbour(size_t aIndex) const { return mNeighbourhood[aIndex]; }

    TileId GetTileId() const { return mTileId; }

    ClientGridNode* GetTileAtPosition(const Ogre::Vector3& aPosistion);

    ClientTile* GetTile() const { return mTile; }
    void SetTileId(TileId aTileId) { }
private:
    std::vector< ClientGridNode* > mNeighbourhood;
    TileId mTileId;
    ClientTile* mTile;
    Ogre::Vector3 mPosition;
};

#endif // CLIENTGRIDNODE_H

