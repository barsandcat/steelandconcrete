#include <pch.h>
#include <ServerBuilding.h>
#include <ServerTile.h>

ServerBuilding::ServerBuilding(ServerTile* aTile, BuildingId aBuildingId): 
    mBuildingId(aBuildingId), mTile(aTile)
{
    mTile->SetBuilding(this);
}

ServerBuilding::~ServerBuilding()
{

}

ServerBuilding::ServerBuilding(const ServerBuilding& other): mBuildingId(-1), mTile(NULL)
{

}

ServerBuilding& ServerBuilding::operator=(const ServerBuilding& rhs)
{
    return *this;
}
