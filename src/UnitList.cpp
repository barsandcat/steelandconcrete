#include <pch.h>
#include <UnitList.h>

#include <ServerUnit.h>

UnitList::UnitVector UnitList::mUnits;
UnitList::FreeIndex UnitList::mFreeIndex;

ServerUnit& UnitList::NewUnit(ServerTile& aTile, const UnitClass& aClass)
{
    if (!mFreeIndex.empty())
    {
        UnitId id = mFreeIndex.back();
        mFreeIndex.pop_back();
        ServerUnit* unit = new ServerUnit(aTile, aClass, id);
        mUnits.replace(id, unit);
        return *unit;
    }
    else
    {
        UnitId id = mUnits.size();
        ServerUnit* unit = new ServerUnit(aTile, aClass, id);
        mUnits.push_back(unit);
        return *unit;
    }

}

void UnitList::DeleteUnit(UnitId aUnitId)
{
    if (!mUnits.is_null(aUnitId))
    {
        mUnits.replace(aUnitId, 0);
        mFreeIndex.push_back(aUnitId);
    }
}

ServerUnit* UnitList::GetUnit(UnitId aUnitId)
{
    if (!mUnits.is_null(aUnitId))
    {
        return &mUnits[aUnitId];
    }
    return NULL;
}
