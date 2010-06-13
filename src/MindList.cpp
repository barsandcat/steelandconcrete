#include <pch.h>
#include <MindList.h>

#include <Mind.h>
#include <ServerUnit.h>
#include <UnitList.h>

MindList::MindListMap MindList::mMinds;

void MindList::NewMind(UnitId aUnitId)
{
    mMinds.insert(aUnitId, new Mind());
}

void MindList::UpdateMinds(GameTime aPeriod)
{
    std::vector<UnitId> deleteList;
    deleteList.reserve(mMinds.size() * 0.1f);
    for (MindListMap::iterator i = mMinds.begin(); i != mMinds.end(); ++i)
    {
        ServerUnit* unit = UnitList::GetUnit(i->first);
        if (!unit)
        {
            deleteList.push_back(i->first);
        }
    }

    for (size_t i = 0; i < deleteList.size(); ++i)
    {
        mMinds.erase(deleteList[i]);
    }
}

void MindList::DeleteMind(UnitId aUnitId)
{
    mMinds.erase(aUnitId);
}

void MindList::Clear()
{
    mMinds.clear();
}

