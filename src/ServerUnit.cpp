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
    ChangeList::AddRemove(mUnitId);
    //dtor
}

void ServerUnit::Move(ServerTile& aNewPosition)
{
    mPosition->SetUnitId(0);
    mPosition = &aNewPosition;
    mPosition->SetUnitId(mUnitId);
    ChangeList::AddMove(mUnitId, mPosition->GetTileId());
}

void ServerUnit::ExecuteCommand()
{
    if(mClass.GetMaxSpeed() > 0)
    {
        if(mTarget && !UnitList::GetUnit(mTarget->GetUnitId()))
        {
            Move(*mTarget);
            mTarget = NULL;
            ChangeList::AddCommandDone(mUnitId);
        }
    }
}

void ServerUnit::FillUnitMsg(UnitMsg& aUnitMsg) const
{
    aUnitMsg.set_tag(mUnitId);
    aUnitMsg.set_tile(mPosition->GetTileId());
    aUnitMsg.set_visual(mClass.GetVisualCode());
}

bool ServerUnit::UpdateAgeAndIsTimeToDie(GameTime aPeriod)
{
    mAge += aPeriod;
    return mAge > mClass.GetMaxAge() + rand() % mClass.GetMaxAge();
}

