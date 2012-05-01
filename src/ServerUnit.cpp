#include <pch.h>
#include <ServerUnit.h>

#include <ServerTile.h>
#include <ChangeList.h>
#include <MindList.h>
#include <UnitList.h>

ServerUnit::ServerUnit(ServerTile& aTile, const UnitClass& aClass, UnitId aUnitId):
    mUnitId(aUnitId), mClass(aClass), mPosition(&aTile),  mTarget(NULL),
      mAge(0)
{
    mPosition->SetUnitId(mUnitId);
    if (aClass.GetMaxSpeed() > 0)
    {
        MindList::NewMind(aUnitId);
    }
}

ServerUnit::~ServerUnit()
{
    mPosition->SetUnitId(0);
    mPosition->GetChangeList()->AddRemove(mUnitId);
}

void ServerUnit::Move(ServerTile& aNewPosition)
{
    mPosition->GetChangeList()->AddLeave(mUnitId, aNewPosition.GetTileId());
    aNewPosition.GetChangeList()->AddEnter(mUnitId, mClass.GetVisualCode(), mPosition->GetTileId());
    mPosition->SetUnitId(0);
    mPosition = &aNewPosition;
    mPosition->SetUnitId(mUnitId);
}

bool ServerUnit::UpdateAgeAndIsTimeToDie(GameTime aPeriod)
{
    mAge += aPeriod;
    return mAge > mClass.GetMaxAge() + rand() % mClass.GetMaxAge();
}

