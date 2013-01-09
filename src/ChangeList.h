#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <INetwork.h>
#include <boost/circular_buffer.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <IChange.h>

class ChangeList
{
public:
    static const int32 mSize;
    typedef boost::ptr_vector<IChange> TurnChanges;
    ChangeList(): mChanges(mSize) { }
    void AddEnter(UnitId aUnit, uint32 aVisualCode, TileId aFrom);
    void AddLeave(UnitId aUnit, TileId aTo);
    void AddRemove(UnitId aUnit);
    void Write(INetwork& aNetwork, size_t aIndex, VisibleTiles& aVisibleTiles) const;
    void Commit();
    void SetTileId(TileId aTileId) { mTileId = aTileId; }
private:
    boost::circular_buffer<TurnChanges> mChanges;
    TurnChanges mCurrentChanges;
    TileId mTileId;
};

#endif // CHANGELIST_H
