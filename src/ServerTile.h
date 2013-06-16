#ifndef SERVERTILE_H
#define SERVERTILE_H
#include <Typedefs.h>
#include <OgreVector3.h>
#include <ChangeList.h>

class ServerTile: public boost::noncopyable
{
public:
    typedef std::set<UnitId>::const_iterator UnitIterator;
    explicit ServerTile(TileId aId, const Ogre::Vector3& aPosition);
    ~ServerTile();
    void AddNeighbour(ServerTile& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ServerTile& aTile);
    void SortNeighbourhood();

    const Ogre::Vector3& GetPosition() const { return mPosition; }
    size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ServerTile& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }
    UnitIterator GetUnits() const { return mUnitList.begin(); }
    bool IsLastUnit(UnitIterator aIterator) const { return mUnitList.end() == aIterator; }
    void AddUnitId(UnitId aUnitId) { mUnitList.insert(aUnitId); }
    void RemoveUnitId(UnitId aUnitId) { mUnitList.erase(aUnitId); }

    bool CanEnter() const;

    TileId GetTileId() const { return mTileId; }
    ChangeList* GetChangeList() { return &mChangeList; }
    void SetHeight(int32 aHeight) { mHeight = aHeight; mWater = std::max(mHeight - 400, 0); }
    int32 GetHeight() const { return mHeight; }
    int32 GetWater() const { return mWater; }
private:
    std::vector< ServerTile* > mNeighbourhood;
    std::set<UnitId> mUnitList;
    const Ogre::Vector3 mPosition;
    const TileId mTileId;
    int32 mHeight;
    int32 mWater;
    ChangeList mChangeList;
};

#endif // SERVERTILE_H
