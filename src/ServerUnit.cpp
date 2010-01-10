#include <pch.h>
#include <ServerUnit.h>

#include <ServerTile.h>
#include <ChangeList.h>

ServerUnit::ServerUnit(ServerTile& aTile, const UnitClass& aClass, UnitId aUnitId):
    mPosition(&aTile), mUnitId(aUnitId), mTarget(NULL),
    mClass(aClass),  mAge(0)
{
    mPosition->SetUnit(this);
}

ServerUnit::~ServerUnit()
{
    ChangeList::AddRemove(mUnitId);
    //dtor
}

void ServerUnit::Move(ServerTile& aNewPosition)
{
    mPosition->SetUnit(NULL);
    mPosition = &aNewPosition;
    mPosition->SetUnit(this);
    ChangeList::AddMove(mUnitId, mPosition->GetTileId());
}

void ServerUnit::ExecuteCommand()
{
    if(mClass.GetMaxSpeed() > 0)
    {
        if(mTarget && !mTarget->GetUnit())
        {
            Move(*mTarget);
            mTarget = NULL;
            ChangeList::AddCommandDone(mUnitId);
        }
        else
        {
            ServerTile& randomTile = mPosition->GetNeighbour(rand() % mPosition->GetNeighbourCount());
            if(!randomTile.GetUnit())
            {
                Move(randomTile);
            }
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

