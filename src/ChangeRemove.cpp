#include "ChangeRemove.h"

ChangeRemove::ChangeRemove(TileId aUnit): mUnitId(aUnit)
{
}

void ChangeRemove::FillChangeMsg(ChangeMsg& aChange, std::set<TileId>& aVisibleTiles) const
{
    RemoveMsg* command = aChange.mutable_remove();
    command->set_unitid(mUnitId);
}
