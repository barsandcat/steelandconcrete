#ifndef CHANGEENTER_H
#define CHANGEENTER_H

#include <IChange.h>

class ChangeEnter : public IChange
{
public:
    ChangeEnter(UnitId aUnitId, uint32 aVisualCode, TileId aFrom, TileId aTo);
    virtual void FillChangeMsg(ChangeMsg& aChange, VisibleTiles& aVisibleTiles) const;
private:
    const UnitId mUnitId;
    const TileId mFrom;
    const TileId mTo;
    const uint32 mVisualCode;
};

#endif // CHANGEENTER_H
