#ifndef SERVERTILE_H
#define SERVERTILE_H
#include <Typedefs.h>
#include <OgreVector3.h>
#include <ChangeList.h>

class ServerTile: public boost::noncopyable
{
public:
    explicit ServerTile(const Ogre::Vector3& aPosition);
    ~ServerTile();
    void AddNeighbour(ServerTile& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ServerTile& aTile);
    void SortNeighbourhood();

    const Ogre::Vector3& GetPosition() const { return mPosition; }
    size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ServerTile& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }
    UnitId GetUnitId() const { return mUnit; }
    void SetUnitId(UnitId aUnit) { mUnit = aUnit; }

    TileId GetTileId() const { return mTileId; }
    void SetTileId(TileId aTileId) { mTileId = aTileId; mChangeList.SetTileId(aTileId); }
    ChangeList* GetChangeList() { return &mChangeList; }

private:
    std::vector< ServerTile* > mNeighbourhood;
    const Ogre::Vector3 mPosition;
    TileId mTileId;
    UnitId mUnit;
    ChangeList mChangeList;
};

#endif // SERVERTILE_H
