#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <Response.pb.h>
#include <INetwork.h>
#include <boost/circular_buffer.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <IChange.h>

class ChangeList
{
public:
    typedef boost::ptr_vector<IChange> TurnChanges;
    ChangeList(): mChanges(100) { mChanges.push_front(TurnChanges()); }
    void AddEnter(UnitId aUnit, uint32 aVisualCode, TileId aFrom);
    void AddLeave(UnitId aUnit, TileId aTo);
    void AddCommandDone(UnitId aUnit);
    void AddRemove(UnitId aUnit);
    void Write(INetwork& aNetwork, size_t aIndex, VisibleTiles& aVisibleTiles) const;
    void Commit();
    void Clear();
    void SetTileId(TileId aTileId) { mTileId = aTileId; }
private:
    boost::circular_buffer<TurnChanges> mChanges;
    TileId mTileId;
};

void SendChanges(INetwork& aNetwork, VisibleTiles& aVisibleTiles, const GameTime aClientTime, const GameTime aTopTime, const GameTime aTimeStep);

#endif // CHANGELIST_H
