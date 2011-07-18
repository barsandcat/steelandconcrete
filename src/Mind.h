#ifndef MIND_H
#define MIND_H

#include <Typedefs.h>
#include <ServerTile.h>


class Mind
{
public:
    Mind(UnitId aUnitId);
    void Update(GameTime aPeriod);
    void SetCommand(ServerTile& aTile) { mTarget = &aTile; }
    bool IsFree() const { return mIsFree; }
    UnitId GetUnitId() const { return mUnitId; }
    void SetFree(bool aValue) { mIsFree = aValue; }
private:
    const UnitId mUnitId;
    ServerTile* mTarget;
    bool mIsFree;
};

#endif // MIND_H
