#include <pch.h>
#include <ChangeList.h>

#include <ServerLog.h>
#include <Exceptions.h>
#include <ServerGame.h>
#include <ChangeRemove.h>
#include <ChangeList.pb.h>

void ChangeList::AddMove(UnitId aUnit, TileId aPosition)
{
}

void ChangeList::Clear()
{
}

void ChangeList::Commit()
{
    mChanges.push_front(TurnChanges());
}

void ChangeList::Write(INetwork& aNetwork, size_t aIndex) const
{
    ResponseMsg msg;
    msg.set_type(RESPONSE_PART);
    const TurnChanges& turnChanges = mChanges.at(aIndex);
    TurnChanges::const_iterator i = turnChanges.begin();
    for (;i != turnChanges.end(); ++i)
    {
        ChangeMsg* change = msg.add_changes();
        i->FillChangeMsg(*change);
    }
    aNetwork.WriteMessage(msg);
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
}

void ChangeList::AddRemove(UnitId aUnit)
{
    mChanges.front().push_back(new ChangeRemove(aUnit));
}
