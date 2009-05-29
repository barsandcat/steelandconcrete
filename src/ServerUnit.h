#ifndef SERVERUNIT_H
#define SERVERUNIT_H
#include <Typedefs.h>
class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, UnitId aUnitId);
    ServerTile& GetPosition() const { return *mPosition; }
    UnitId GetUnitId() const { return mUnitId; }
    virtual ~ServerUnit();
    void Move(size_t aIndex);
protected:
private:
    ServerTile* mPosition;
    const UnitId mUnitId;
};

#endif // SERVERUNIT_H
