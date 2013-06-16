#ifndef CLIENTGRIDNODE_H
#define CLIENTGRIDNODE_H


class ClientUnit;
class TileEntity;

class ClientTile: public boost::noncopyable
{
public:
    explicit ClientTile(TileId aId, const Ogre::Vector3& aPosition);
    ~ClientTile();

    void CreateEntity(bool ground);
    TileEntity* GetTile() const { return mTile; }
    void DestroyEntity();

    void AddNeighbour(ClientTile& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ClientTile& aTile);
    void SortNeighbourhood();
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientTile& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }

    TileId GetTileId() const { return mTileId; }
    Ogre::Vector3 GetPosition() const { return mPosition; }
    ClientTile* GetTileAtPosition(const Ogre::Vector3& aPosistion);

    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { assert(aUnit); mUnit = aUnit; }
    void RemoveUnit() { mUnit = NULL; }
private:
    std::vector< ClientTile* > mNeighbourhood;
    const TileId mTileId;
    TileEntity* mTile;
    ClientUnit* mUnit;
    Ogre::Vector3 mPosition;
};

#endif // CLIENTGRIDNODE_H

