#ifndef CHANGEREMOVE_H
#define CHANGEREMOVE_H

#include <IChange.h>
#include <Typedefs.h>

class ChangeRemove: public IChange
{
public:
    ChangeRemove(TileId aUnit);
    virtual void FillChangeMsg(ChangeMsg& aChange, VisibleTiles& aVisibleTiles) const;
private:
    const TileId mUnitId;
};

#endif // CHANGEREMOVE_H
