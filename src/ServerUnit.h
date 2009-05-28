#ifndef SERVERUNIT_H
#define SERVERUNIT_H
#include <Typedefs.h>

class ServerUnit
{
public:
    ServerUnit(TileId aTile, UnitId aUnitId);
    void SetPosition(TileId aTile) { mPosition = aTile; }
    TileId GetPosition() const { return mPosition; }
    UnitId GetUnitId() const { return mUnitId; }
    virtual ~ServerUnit();
    void Update();
protected:
private:
    TileId mPosition;
    UnitId mUnitId;
};

#endif // SERVERUNIT_H
