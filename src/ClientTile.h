#ifndef CLIENTGRIDNODE_H
#define CLIENTGRIDNODE_H


class ClientUnit;
class TileEntity;

class ClientTile: public boost::noncopyable
{
public:
    typedef std::set<UnitId>::const_iterator UnitIterator;
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

    UnitIterator GetUnits() const { return mUnits.begin(); }
    bool IsLastUnit(UnitIterator aIterator) const { return aIterator == mUnits.end(); }
    void AddUnit(UnitId aUnit) { mUnits.insert(aUnit); }
    void RemoveUnit(UnitId aUnit) { mUnits.erase(aUnit); }
private:
    std::vector< ClientTile* > mNeighbourhood;
    std::set<UnitId> mUnits;
    const TileId mTileId;
    TileEntity* mTile;
    Ogre::Vector3 mPosition;
};

#endif // CLIENTGRIDNODE_H

