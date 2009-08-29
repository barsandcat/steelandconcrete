#include <pch.h>
#include <ChangeList.h>

#include <Network.h>

ResponseMsg ChangeList::mChangeList;

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
void ChangeList::Write(socket_t& aSocket, GameTime aTime)
{
	mChangeList.set_type(Changes);
	mChangeList.set_last(true);
	mChangeList.set_time(aTime);
    WriteMessage(aSocket, mChangeList);
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
    ChangeMsg* change = mChangeList.add_changes();
    CommandDoneMsg* command = change->mutable_commanddone();
    command->set_unitid(aUnit);
}
