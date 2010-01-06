#include <pch.h>
#include <ChangeList.h>

#include <ServerLog.h>

std::list< ResponseMsg > ChangeList::mChangeList;

void ChangeList::AddChangeMsg(ChangeMsg& aChange)
{
    if (!mChangeList.empty() && (mChangeList.front().changes_size() < 250))
    {
        *(mChangeList.front().add_changes()) = aChange;
    }
    else
    {
        ResponseMsg msg;
        msg.set_type(RESPONSE_CHANGES);
        msg.set_last(true);
        if (!mChangeList.empty())
        {
            mChangeList.front().set_last(false);
        }
        mChangeList.push_front(msg);
        *(msg.add_changes()) = aChange;
    }

}

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
    ChangeMsg change;
    UnitMoveMsg* move = change.mutable_unitmove();
    move->set_unitid(aUnit);
    move->set_position(aPosition);
    AddChangeMsg(change);
}

void ChangeList::Clear()
{
    mChangeList.clear();
}
void ChangeList::Write(INetwork& aNetwork, GameTime aTime)
{
    if (!mChangeList.empty())
    {
        while (!mChangeList.empty())
        {
            ResponseMsg& msg = mChangeList.back();
            msg.set_time(aTime);
            aNetwork.WriteMessage(msg);
            mChangeList.pop_back();
        }
    }
    else
    {
        ResponseMsg msg;
        msg.set_type(RESPONSE_CHANGES);
        msg.set_last(true);
        msg.set_time(aTime);
        aNetwork.WriteMessage(msg);
    }
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
    ChangeMsg change;
    CommandDoneMsg* command = change.mutable_commanddone();
    command->set_unitid(aUnit);
    AddChangeMsg(change);
}

void ChangeList::AddRemove(UnitId aUnit)
{
    ChangeMsg change;
    RemoveMsg* command = change.mutable_remove();
    command->set_unitid(aUnit);
    AddChangeMsg(change);
}
