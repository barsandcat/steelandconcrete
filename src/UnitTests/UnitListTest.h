#ifndef UNITLISTTEST_H_INCLUDED
#define UNITLISTTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <UnitList.h>
#include <ServerTile.h>
#include <OgreVector3.h>
#include <UnitClass.h>
#include <ServerUnit.h>

class UnitListTest: public CxxTest::TestSuite
{
public:
    void TestBase()
    {
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);
        ServerUnit& unit = UnitList::NewUnit(tile, unitClass);
        UnitId unitId = unit.GetUnitId();
        ServerUnit* unit2 = UnitList::GetUnit(unitId);
        TS_ASSERT_EQUALS(&unit, unit2);
        UnitList::DeleteUnit(unitId);
        TS_ASSERT(!UnitList::GetUnit(unitId));
    }

};

#endif // UNITLISTTEST_H_INCLUDED
