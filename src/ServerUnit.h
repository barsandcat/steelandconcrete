#ifndef SERVERUNIT_H
#define SERVERUNIT_H
#include <Typedefs.h>
#include <Unit.pb.h>

class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, UnitId aUnitId, uint32 aVisualCode);
    ServerTile& GetPosition() const { return *mPosition; }
    UnitId GetUnitId() const { return mUnitId; }
    virtual ~ServerUnit();
    void Move(ServerTile& aNewPosition);
    void SetCommand(ServerTile& aTile) { mTarget = &aTile; }
    void ExecuteCommand();
    void SetMaster(UnitId aMaster) { mMaster = aMaster; }
    void FillUnitMsg(UnitMsg& aUnitMsg) const;
    bool UpdateAgeAndIsTimeToDie(GameTime aPeriod);
protected:
private:
    ServerTile* mPosition;
    const UnitId mUnitId;
    ServerTile* mTarget;
    UnitId mMaster;
    uint32 mVisualCode;
    uint32 mAge;
};

#endif // SERVERUNIT_H
