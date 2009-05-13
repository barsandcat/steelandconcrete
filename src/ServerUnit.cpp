#include <ServerUnit.h>
#include <ServerTile.h>
ServerUnit::ServerUnit(ServerTile& aTile, int aIndex): mTile(&aTile), mIndex(aIndex)
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
