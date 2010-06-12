#ifndef UNITLIST_H
#define UNITLIST_H

#include <Typedefs.h>

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
    static void Clear();
private:
    typedef std::vector< ServerUnit* > UnitVector;
    typedef std::list< UnitId > FreeIdList;
    static const int32 INDEX_SIZE = 16;
    static const int32 INDEX_MASK = 0x0000FFFF;
    static UnitVector mUnits;
    static FreeIdList mFreeIdList;
};

#endif // UNITLIST_H
