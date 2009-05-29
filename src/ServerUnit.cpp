#include <ServerUnit.h>

ServerUnit::ServerUnit(TileId aTile, UnitId aUnitId):
  mPosition(aTile, mChangeList), mUnitId(aUnitId)
{
    //ctor
}

ServerUnit::~ServerUnit()
{
    //dtor
}

void ServerUnit::Update()
{

}
