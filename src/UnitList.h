#ifndef UNITLIST_H
#define UNITLIST_H

#include <Typedefs.h>
#include <boost/ptr_container/ptr_vector.hpp>

class ServerUnit;
class ServerTile;
class UnitClass;

class UnitList
{
public:
    static ServerUnit& NewUnit(ServerTile& aTile, const UnitClass& aClass);
    static void DeleteUnit(UnitId aUnitId);
    static ServerUnit* GetUnit(UnitId aUnitId);
    static int32 GetSize() { return mUnits.size(); }
    static void Clear() { mUnits.clear(); }
private:
    typedef boost::ptr_vector< boost::nullable< ServerUnit> > UnitVector;
    static UnitVector mUnits;
};

#endif // UNITLIST_H
