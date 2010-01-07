#ifndef SERVERUNIT_H
#define SERVERUNIT_H
#include <Typedefs.h>
#include <Unit.pb.h>

class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, UnitId aUnitId, uint32 aVisualCode, uint32 aMaxAge);
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
    const UnitId mUnitId;
    const uint32 mVisualCode;
    const uint32 mMaxAge;
    ServerTile* mPosition;
    ServerTile* mTarget;
    UnitId mMaster;
    uint32 mAge;
};

#endif // SERVERUNIT_H
