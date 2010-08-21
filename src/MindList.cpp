#include <pch.h>
#include <MindList.h>

#include <Mind.h>
#include <ServerUnit.h>
#include <UnitList.h>
#include <Exceptions.h>

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

UnitId MindList::GetAvatar()
{
    if (mMinds.empty())
    {
        boost::throw_exception(NoAvatar());
    }
    else
    {
        MindListMap::iterator i = mMinds.begin();
        UnitId id = i->first;
        mMinds.erase(i);
        return id;
    }
}

void MindList::Clear()
{
    mMinds.clear();
}

