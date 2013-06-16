#ifndef CLIENTGRIDNODE_H
#define CLIENTGRIDNODE_H


class ClientUnit;
class TileEntity;

class ClientGridNode: public boost::noncopyable
{
public:
    explicit ClientGridNode(TileId aId, const Ogre::Vector3& aPosition);
    ~ClientGridNode();

    void CreateTile(bool ground);
    TileEntity* GetTile() const { return mTile; }
    void DestroyTile();

    void AddNeighbour(ClientGridNode& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ClientGridNode& aTile);
    void SortNeighbourhood();
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientGridNode& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }

    TileId GetTileId() const { return mTileId; }
    Ogre::Vector3 GetPosition() const { return mPosition; }
    ClientGridNode* GetTileAtPosition(const Ogre::Vector3& aPosistion);

    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { assert(aUnit); mUnit = aUnit; }
    void RemoveUnit() { mUnit = NULL; }
private:
    std::vector< ClientGridNode* > mNeighbourhood;
    const TileId mTileId;
    TileEntity* mTile;
    ClientUnit* mUnit;
    Ogre::Vector3 mPosition;
};

#endif // CLIENTGRIDNODE_H

