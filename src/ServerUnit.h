#ifndef SERVERUNIT_H
#define SERVERUNIT_H

#include <Typedefs.h>
#include <UnitClass.h>

class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, const UnitClass& aClass, UnitId aUnitId);
    ServerTile& GetUnitTile() const { return *mPosition; }
    UnitId GetUnitId() const { return mUnitId; }
    virtual ~ServerUnit();
    const UnitClass& GetClass() const { return mClass; }
    void Move(ServerTile& aNewPosition);
protected:
private:
    const UnitId mUnitId;
    const UnitClass& mClass;
    ServerTile* mPosition;
    ServerTile* mTarget;
};

#endif // SERVERUNIT_H
