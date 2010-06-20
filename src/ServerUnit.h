#ifndef SERVERUNIT_H
#define SERVERUNIT_H

#include <Typedefs.h>
#include <Unit.pb.h>
#include <UnitClass.h>

class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, const UnitClass& aClass, UnitId aUnitId);
    ServerTile& GetPosition() const { return *mPosition; }
    UnitId GetUnitId() const { return mUnitId; }
    virtual ~ServerUnit();
    void SetCommand(ServerTile& aTile) { mTarget = &aTile; }
    void ExecuteCommand();
    void FillUnitMsg(UnitMsg& aUnitMsg) const;
    bool UpdateAgeAndIsTimeToDie(GameTime aPeriod);
protected:
private:
    void Move(ServerTile& aNewPosition);
    const UnitId mUnitId;
    const UnitClass& mClass;
    ServerTile* mPosition;
    ServerTile* mTarget;
    uint32 mAge;
};

#endif // SERVERUNIT_H
