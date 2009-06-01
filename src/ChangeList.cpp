#include <pch.h>
#include <ChangeList.h>

#include <Network.h>

ChangeListMsg ChangeList::mChangeList;

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
    ChangeMsg* change = mChangeList.add_changes();
    UnitMoveMsg* move = change->mutable_unitmove();
    move->set_unitid(aUnit);
    move->set_position(aPosition);
}

void ChangeList::Clear()
{
    mChangeList.Clear();
}
void ChangeList::Write(socket_t& aSocket)
{
    mChangeList.set_last(true);
    WriteMessage(aSocket, mChangeList);
}
