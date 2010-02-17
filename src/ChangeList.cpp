#include <pch.h>
#include <ChangeList.h>

#include <ServerLog.h>

std::list< ResponseMsg* > ChangeList::mChangeList;
GameTime ChangeList::mTime;

ChangeMsg* ChangeList::AddChangeMsg()
{
    ChangeMsg* change = NULL;
    if (!mChangeList.empty() && (mChangeList.front()->changes_size() < 250))
    {
        change = mChangeList.front()->add_changes();
    }
    else
    {
        ResponseMsg* msg = new ResponseMsg();
        msg->set_type(RESPONSE_CHANGES);
        msg->set_last(true);
        change = msg->add_changes();

        if (!mChangeList.empty())
        {
            mChangeList.front()->set_last(false);
        }
        mChangeList.push_front(msg);
    }
	return change;
}

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
    ChangeMsg* change = AddChangeMsg();
    UnitMoveMsg* move = change->mutable_unitmove();
    move->set_unitid(aUnit);
    move->set_position(aPosition);
}

void ChangeList::Clear()
{
    for (std::list< ResponseMsg* >::iterator i = mChangeList.begin(); i != mChangeList.end(); ++i)
    {
        delete *i;
    }
    mChangeList.clear();
}

void ChangeList::SetTime(GameTime aTime)
{
    for (std::list< ResponseMsg* >::iterator i = mChangeList.begin(); i != mChangeList.end(); ++i)
    {
        (*i)->set_time(aTime);
    }
    mTime = aTime;
}

void ChangeList::Write(INetwork& aNetwork)
{
    if (!mChangeList.empty())
    {
        std::list< ResponseMsg* >::const_iterator i = mChangeList.end();
        do
        {
            --i;
            aNetwork.WriteMessage(**i);
        }
        while (i != mChangeList.begin());
    }
    else
    {
        ResponseMsg emptyMsg;
        emptyMsg.set_type(RESPONSE_CHANGES);
        emptyMsg.set_last(true);
        emptyMsg.set_time(mTime);
        aNetwork.WriteMessage(emptyMsg);
    }
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
    ChangeMsg* change = AddChangeMsg();
    CommandDoneMsg* command = change->mutable_commanddone();
    command->set_unitid(aUnit);
}

void ChangeList::AddRemove(UnitId aUnit)
{
    ChangeMsg* change = AddChangeMsg();
    RemoveMsg* command = change->mutable_remove();
    command->set_unitid(aUnit);
}
