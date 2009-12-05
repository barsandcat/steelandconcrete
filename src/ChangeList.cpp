#include <pch.h>
#include <ChangeList.h>

#include <Network.h>

std::list< ResponseMsg > ChangeList::mChangeList;

ChangeMsg* ChangeList::GetChangeMsg()
{
    if (!mChangeList.empty() && (mChangeList.front().ByteSize() + 20 < MESSAGE_SIZE))
    {
        return mChangeList.front().add_changes();
    }
    else
    {
        ResponseMsg msg;
        msg.set_type(RESPONSE_CHANGES);
        mChangeList.push_front(msg);
        return msg.add_changes();
    }

}

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
    ChangeMsg* change = GetChangeMsg();
    UnitMoveMsg* move = change->mutable_unitmove();
    move->set_unitid(aUnit);
    move->set_position(aPosition);
}

void ChangeList::Clear()
{
    mChangeList.clear();
}
void ChangeList::Write(socket_t& aSocket, GameTime aTime)
{
    if (!mChangeList.empty())
    {
        while (!mChangeList.empty())
        {
            ResponseMsg& msg = mChangeList.front();
            msg.set_type(RESPONSE_CHANGES);
            msg.set_last(mChangeList.begin() == mChangeList.end());
            msg.set_time(aTime);
            WriteMessage(aSocket, msg);
            mChangeList.pop_front();
        }
    }
    else
    {
        ResponseMsg msg;
        msg.set_type(RESPONSE_CHANGES);
        msg.set_last(true);
        msg.set_time(aTime);
        WriteMessage(aSocket, msg);
    }
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
    ChangeMsg* change = GetChangeMsg();
    CommandDoneMsg* command = change->mutable_commanddone();
    command->set_unitid(aUnit);
}

void ChangeList::AddRemove(UnitId aUnit)
{
    ChangeMsg* change = GetChangeMsg();
    RemoveMsg* command = change->mutable_remove();
    command->set_unitid(aUnit);
}
