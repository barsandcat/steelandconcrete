#include <pch.h>
#include <UnitList.h>

#include <ServerUnit.h>
#include <UnitListIterator.h>

UnitList::UnitVector UnitList::mUnits;
UnitList::FreeIdList UnitList::mFreeIdList;

ServerUnit& UnitList::NewUnit(ServerTile& aTile, const UnitClass& aClass)
{
    if (!mFreeIdList.empty())
    {
        UnitId freeId = mFreeIdList.front();
        mFreeIdList.pop_front();
        ServerUnit* unit = new ServerUnit(aTile, aClass, freeId + (1 << INDEX_SIZE));
        assert(!mUnits[freeId & INDEX_MASK]);
        mUnits[freeId & INDEX_MASK] = unit;
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
    int32 index = aUnitId & INDEX_MASK;
    ServerUnit* unit = mUnits[index];
    if (unit)
    {
        delete unit;
        mUnits[index] = 0;
        mFreeIdList.push_back(aUnitId);
    }
}

ServerUnit* UnitList::GetUnit(UnitId aUnitId)
{
    ServerUnit* unit = mUnits[aUnitId & INDEX_MASK];
    return unit && unit->GetUnitId() == aUnitId ? unit : 0;
}

void UnitList::Clear()
{
    for (size_t i = 0; i < mUnits.size(); ++i)
    {
        delete mUnits[i];
    }
    mUnits.clear();
    mFreeIdList.clear();
}

UnitListIterator UnitList::GetIterator()
{
    return UnitListIterator(mUnits);
}
