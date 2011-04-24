#ifndef CHANGEREMOVE_H
#define CHANGEREMOVE_H

#include <Change.h>
#include <Typedefs.h>

class ChangeRemove: public IChange
{
public:
    ChangeRemove(TileId aUnit);
    virtual void FillChangeMsg(ChangeMsg& aChange) const;
private:
    TileId mUnitId;
};

#endif // CHANGEREMOVE_H
