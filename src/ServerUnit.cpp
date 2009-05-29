#include <ServerUnit.h>
#include <ServerTile.h>

ServerUnit::ServerUnit(ServerTile& aTile, UnitId aUnitId):
  mPosition(&aTile), mUnitId(aUnitId)
{
    //ctor
}

ServerUnit::~ServerUnit()
{
    //dtor
}

void ServerUnit::Move(size_t aIndex)
{
    ServerTile& newPosition = mPosition->GetNeighbour(aIndex);
    assert(newPosition.GetUnit() == NULL);
    mPosition->SetUnit(NULL);
    mPosition = &newPosition;
    mPosition->SetUnit(this);
}
