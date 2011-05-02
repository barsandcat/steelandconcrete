#include <pch.h>
#include <ChangeList.h>

#include <Exceptions.h>
#include <ServerGame.h>
#include <ChangeRemove.h>
#include <ChangeLeave.h>
#include <ChangeEnter.h>
#include <ChangeList.pb.h>


void SendChanges(INetwork& aNetwork, VisibleTiles& aVisibleTiles, const GameTime aClientTime, const GameTime aTopTime, const GameTime aTimeStep)
{
    const int32 toSend = (aTopTime - aClientTime) / aTimeStep;
    for (int32 t = toSend - 1; t >= 0; --t)
    {
        for (VisibleTiles::const_iterator i = aVisibleTiles.begin(); i != aVisibleTiles.end(); ++i)
        {
            i->second->Write(aNetwork, t, aVisibleTiles);
        }
    }
}

void ChangeList::AddEnter(UnitId aUnit, uint32 aVisualCode, TileId aFrom)
{
    mChanges.front().push_back(new ChangeEnter(aUnit, aVisualCode, aFrom, mTileId));
}

void ChangeList::AddLeave(UnitId aUnit, TileId aTo)
{
    mChanges.front().push_back(new ChangeLeave(aUnit, aTo));
}


void ChangeList::Clear()
{
}

void ChangeList::Commit()
{
    mChanges.push_front(TurnChanges());
}

void ChangeList::Write(INetwork& aNetwork, size_t aIndex, VisibleTiles& aVisibleTiles) const
{
    ResponseMsg msg;
    msg.set_type(RESPONSE_PART);
    const TurnChanges& turnChanges = mChanges.at(aIndex);
    TurnChanges::const_iterator i = turnChanges.begin();
    for (;i != turnChanges.end(); ++i)
    {
        ChangeMsg* change = msg.add_changes();
        i->FillChangeMsg(*change, aVisibleTiles);
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
