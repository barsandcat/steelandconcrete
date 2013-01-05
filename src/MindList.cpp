#include <pch.h>
#include <MindList.h>

#include <Mind.h>
#include <ServerUnit.h>
#include <UnitList.h>

MindList::MindListMap MindList::mMinds;

void MindList::NewMind(UnitId aUnitId)
{
    mMinds.insert(aUnitId, new Mind(aUnitId));
}

void MindList::UpdateMinds(GameTime aPeriod)
{
    std::vector<UnitId> deleteList;
    deleteList.reserve(mMinds.size() * 0.1f);
    for (MindListMap::iterator i = mMinds.begin(); i != mMinds.end(); ++i)
    {
        ServerUnit* unit = UnitList::GetUnit(i->first);
        if (unit)
        {
            i->second->Update(aPeriod);
        }
        else
        {
            deleteList.push_back(i->first);
        }
    }

    for (size_t i = 0; i < deleteList.size(); ++i)
    {
        mMinds.erase(deleteList[i]);
    }
}

Mind* MindList::GetFreeMind()
{
    MindListMap::iterator i = mMinds.begin();
    Mind* freeMind = NULL;
    while (i != mMinds.end() && freeMind == NULL)
    {
        Mind* mind = i->second;
        if (mind->IsFree())
            freeMind = mind;
        ++i;
    }
    return freeMind;
}

void MindList::Clear()
{
    mMinds.clear();
}

