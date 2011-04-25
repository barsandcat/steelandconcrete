#include "ChangeLeave.h"

void ChangeLeave::FillChangeMsg(ChangeMsg& aChange, std::set<TileId>& aVisibleTiles) const
{
    if (aVisibleTiles.find(mTo) == aVisibleTiles.end())
    {
        UnitLeaveMsg* command = aChange.mutable_unitleave();
        command->set_unitid(mUnitId);
        command->set_to(mTo);
    }
}
