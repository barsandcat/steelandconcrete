#include <pch.h>
#include <ServerUnit.h>

#include <ServerTile.h>
#include <ChangeList.h>
#include <MindList.h>
#include <UnitList.h>

ServerUnit::ServerUnit(ServerTile& aTile, const UnitClass& aClass, UnitId aUnitId):
    mUnitId(aUnitId), mClass(aClass), mPosition(&aTile),  mTarget(NULL)
{
    mPosition->AddUnitId(mUnitId);
    if (aClass.GetMaxSpeed() > 0)
    {
        MindList::NewMind(aUnitId);
    }
}

ServerUnit::~ServerUnit()
{
    mPosition->RemoveUnitId(mUnitId);
    mPosition->GetChangeList()->AddRemove(mUnitId);
}

void ServerUnit::Move(ServerTile& aNewPosition)
{
    mPosition->GetChangeList()->AddLeave(mUnitId, aNewPosition.GetTileId());
    aNewPosition.GetChangeList()->AddEnter(mUnitId, mClass.GetVisualCode(), mPosition->GetTileId());
    mPosition->RemoveUnitId(mUnitId);
    mPosition = &aNewPosition;
    mPosition->AddUnitId(mUnitId);
}

