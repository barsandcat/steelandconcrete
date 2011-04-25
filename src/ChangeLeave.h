#ifndef CHANGELEAVE_H
#define CHANGELEAVE_H

#include <IChange.h>


class ChangeLeave : public IChange
{
public:
    ChangeLeave(UnitId aUnitId, TileId aTo): mUnitId(aUnitId), mTo(aTo) {}
    virtual void FillChangeMsg(ChangeMsg& aChange, std::set<TileId>& aVisibleTiles) const;
private:
    const UnitId mUnitId;
    const TileId mTo;
};

#endif // CHANGELEAVE_H
