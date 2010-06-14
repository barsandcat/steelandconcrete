#include <pch.h>
#include <Mind.h>

#include <ServerUnit.h>
#include <ServerTile.h>
#include <UnitList.h>

Mind::Mind(UnitId aUnitId): mUnitId(aUnitId)
{
    //ctor
}

void Mind::Update(GameTime aPeriod)
{
    ServerUnit* unit = UnitList::GetUnit(mUnitId);
    ServerTile& position = unit->GetPosition();
    ServerTile& randomTile = position.GetNeighbour(rand() % position.GetNeighbourCount());
    if(!UnitList::GetUnit(randomTile.GetUnitId()))
    {
        unit->SetCommand(randomTile);
    }
}
