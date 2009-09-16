#ifndef SERVERBUILDING_H
#define SERVERBUILDING_H

class ServerTile;

class ServerBuilding
{
public:
    ServerBuilding(ServerTile* aTile, BuildingId aBuildingId);
    virtual ~ServerBuilding();
    BuildingId GetBuildingId() const { return mBuildingId; }
protected:
private:
    ServerBuilding(const ServerBuilding& other);
    ServerBuilding& operator=(const ServerBuilding& other);
    const BuildingId mBuildingId;
    ServerTile* const mTile;
};

#endif // SERVERBUILDING_H
