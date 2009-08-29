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
    void Move(ServerTile& aNewPosition);
    void SetCommand(ServerTile& aTile) { mTarget = &aTile; }
    void ExecuteCommand();
protected:
private:
    ServerTile* mPosition;
    const UnitId mUnitId;
    ServerTile* mTarget;
};

#endif // SERVERUNIT_H
