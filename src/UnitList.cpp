#include <pch.h>
#include <UnitList.h>

#include <ServerUnit.h>

UnitList::UnitVector UnitList::mUnits;

ServerUnit& UnitList::NewUnit(ServerTile& aTile, const UnitClass& aClass)
{
    UnitId id = mUnits.size();
    ServerUnit* unit = new ServerUnit(aTile, aClass, id);
    mUnits.push_back(unit);
    return *unit;
}

void UnitList::DeleteUnit(UnitId aUnitId)
{
    mUnits.replace(aUnitId, 0);
}

ServerUnit* UnitList::GetUnit(UnitId aUnitId)
{
    if (!mUnits.is_null(aUnitId))
    {
        return &mUnits[aUnitId];
    }
    return NULL;
}
