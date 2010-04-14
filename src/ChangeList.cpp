#include <pch.h>
#include <ChangeList.h>

#include <ServerLog.h>

ChangeList::UpdateBlockList ChangeList::mChangeList;
GameTime ChangeList::mTime;

ChangeMsg& ChangeList::AddChangeMsg()
{
    if (mChangeList.empty())
    {
        mChangeList.push_back(std::make_pair(0, ResponseList()));
    }
    ResponseList& changeList = mChangeList.back().second;

    ChangeMsg* change = NULL;
    if (!changeList.empty() && (changeList.back()->changes_size() < 250))
    {
        change = changeList.back()->add_changes();
    }
    else
    {
        ResponseMsg* msg = new ResponseMsg();
        msg->set_type(RESPONSE_PART);
        change = msg->add_changes();
        changeList.push_back(msg);
    }
    assert(change);
    return *change;
}

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
    ChangeMsg& change = AddChangeMsg();
    UnitMoveMsg* move = change.mutable_unitmove();
    move->set_unitid(aUnit);
    move->set_position(aPosition);
}

void ChangeList::Clear()
{
    for (UpdateBlockList::iterator i = mChangeList.begin(); i != mChangeList.end(); ++i)
    {
        ResponseList responseList = i->second;
        for (ResponseList::iterator j = responseList.begin(); j != responseList.end(); ++j)
        {
            delete *j;
        }
    }
    mChangeList.clear();
}

void ChangeList::SetTime(GameTime aTime)
{
    mChangeList.push_back(std::make_pair(aTime, ResponseList()));

    if (mChangeList.size() > 200)
    {
        ResponseList& changeList = mChangeList.front().second;
        for (ResponseList::iterator i = changeList.begin(); i != changeList.end(); ++i)
        {
            delete *i;
        }
        mChangeList.pop_front();
    }

    mTime = aTime;
}

bool ChangeBlockCmp(ChangeList::UpdateBlock i, ChangeList::UpdateBlock j)
{
    return i.first < j.first;
}

void ChangeList::Write(INetwork& aNetwork, GameTime aClientTime)
{
    UpdateBlockList::iterator i =
        std::lower_bound(mChangeList.begin(), mChangeList.end(),
                         std::make_pair(aClientTime, ResponseList()), ChangeBlockCmp);
    while (i != mChangeList.end())
    {
        ResponseList& changeList = i->second;
        if (!changeList.empty())
        {
            ResponseList::const_iterator j = changeList.begin();
            while (j != changeList.end())
            {
                aNetwork.WriteMessage(**j);
                ++j;
            }
        }
        ++i;
    }

    ResponseMsg emptyMsg;
    emptyMsg.set_type(RESPONSE_OK);
    emptyMsg.set_time(mTime);
    aNetwork.WriteMessage(emptyMsg);
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
    ChangeMsg& change = AddChangeMsg();
    CommandDoneMsg* command = change.mutable_commanddone();
    command->set_unitid(aUnit);
}

void ChangeList::AddRemove(UnitId aUnit)
{
    ChangeMsg& change = AddChangeMsg();
    RemoveMsg* command = change.mutable_remove();
    command->set_unitid(aUnit);
}
