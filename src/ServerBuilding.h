#ifndef SERVERBUILDING_H
#define SERVERBUILDING_H


class ServerBuilding
{
public:
    ServerBuilding(BuildingId aBuildingId);
    virtual ~ServerBuilding();
protected:
private:
    ServerBuilding(const ServerBuilding& other);
    ServerBuilding& operator=(const ServerBuilding& other);
    BuildingId mBuildingId;
};

#endif // SERVERBUILDING_H
