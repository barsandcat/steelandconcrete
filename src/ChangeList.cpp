#include <pch.h>
#include <ChangeList.h>

#include <Exceptions.h>
#include <ServerGame.h>
#include <ChangeRemove.h>
#include <ChangeLeave.h>
#include <ChangeEnter.h>
#include <ChangeList.pb.h>

const int32 ChangeList::mSize = 100;


void ChangeList::AddEnter(UnitId aUnit, uint32 aVisualCode, TileId aFrom)
{
    mCurrentChanges.push_back(new ChangeEnter(aUnit, aVisualCode, aFrom, mTileId));
}

void ChangeList::AddLeave(UnitId aUnit, TileId aTo)
{
    mCurrentChanges.push_back(new ChangeLeave(aUnit, aTo));
}

void ChangeList::Clear()
{
}

void ChangeList::Commit()
{
    mChanges.push_front(TurnChanges());
    TurnChanges& front = mChanges.front();
    front.transfer(front.end(), mCurrentChanges.begin(), mCurrentChanges.end(), mCurrentChanges);
}

void ChangeList::Write(INetwork& aNetwork, size_t aIndex, VisibleTiles& aVisibleTiles) const
{
    const TurnChanges& turnChanges = mChanges.at(aIndex);
    if (!turnChanges.empty())
    {
        PayloadMsg msg;
        msg.set_last(false);
        TurnChanges::const_iterator i = turnChanges.begin();
        for (;i != turnChanges.end(); ++i)
        {
            ChangeMsg* change = msg.add_changes();
            i->FillChangeMsg(*change, aVisibleTiles);
        }
        aNetwork.WriteMessage(msg);
    }
}

void ChangeList::AddCommandDone(UnitId aUnit)
{
}

void ChangeList::AddRemove(UnitId aUnit)
{
    mCurrentChanges.push_back(new ChangeRemove(aUnit));
}
