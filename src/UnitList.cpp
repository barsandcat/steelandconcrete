#include <pch.h>
#include <UnitList.h>

#include <ServerUnit.h>
#include <UnitListIterator.h>

UnitList::UnitVector UnitList::mUnits;
UnitList::FreeIdList UnitList::mFreeIdList;
int32 UnitList::mCount = 0;

ServerUnit& UnitList::NewUnit(ServerTile& aTile, const UnitClass& aClass)
{
    ++mCount;
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
        UnitId id = mUnits.size() + (1 << INDEX_SIZE);
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
        --mCount;
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
    mCount = 0;
}

UnitListIterator UnitList::GetIterator()
{
    return UnitListIterator(mUnits);
}
