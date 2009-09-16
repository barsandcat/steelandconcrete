#include <pch.h>
#include <ServerUnit.h>

#include <ServerTile.h>
#include <ChangeList.h>

ServerUnit::ServerUnit(ServerTile& aTile, UnitId aUnitId):
  mPosition(&aTile), mUnitId(aUnitId), mTarget(NULL)
{
    mPosition->SetUnit(this);
}

ServerUnit::~ServerUnit()
{
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
    if (mTarget && !mTarget->GetUnit())
    {
        Move(*mTarget);
        mTarget = NULL;
        ChangeList::AddCommandDone(mUnitId);
    }
}
