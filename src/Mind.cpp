#include <pch.h>
#include <Mind.h>

#include <ServerUnit.h>
#include <ServerTile.h>
#include <UnitList.h>

Mind::Mind(UnitId aUnitId): mUnitId(aUnitId), mIsFree(true)
{
    //ctor
}

void Mind::Update(GameTime aPeriod)
{
    ServerUnit* unit = UnitList::GetUnit(mUnitId);
    if(mTarget && !UnitList::GetUnit(mTarget->GetUnitId()))
    {
        unit->Move(*mTarget);
        mTarget = NULL;
        //ChangeList::AddCommandDone(mUnitId);
    }
    else
    {
        ServerTile& position = unit->GetPosition();
        ServerTile& randomTile = position.GetNeighbour(rand() % position.GetNeighbourCount());
        if(!UnitList::GetUnit(randomTile.GetUnitId()))
        {
            unit->Move(randomTile);
        }
    }
}
