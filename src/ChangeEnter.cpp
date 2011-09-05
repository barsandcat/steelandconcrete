#include <pch.h>
#include <ChangeEnter.h>

#include <UnitList.h>
#include <ServerUnit.h>

ChangeEnter::ChangeEnter(UnitId aUnitId, uint32 aVisualCode, TileId aFrom, TileId aTo):
    mUnitId(aUnitId), mFrom(aFrom), mTo(aTo),
    mVisualCode(aVisualCode)
{

}

void ChangeEnter::FillChangeMsg(ChangeMsg& aChange, VisibleTiles& aVisibleTiles) const
{
    UnitEnterMsg* msg = aChange.mutable_unitenter();
    msg->set_unitid(mUnitId);
    msg->set_to(mTo);
    if (aVisibleTiles.find(mFrom) == aVisibleTiles.end())
    {
        msg->set_visualcode(mVisualCode);
    }
}
