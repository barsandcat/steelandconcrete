#ifndef UNITLISTTEST_H_INCLUDED
#define UNITLISTTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <UnitList.h>
#include <ServerTile.h>
#include <OgreVector3.h>
#include <UnitClass.h>
#include <ServerUnit.h>
#include <UnitListIterator.h>

class UnitListTest: public CxxTest::TestSuite
{
public:

    void TestBase()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        ServerUnit& unit = UnitList::NewUnit(tile, unitClass);
        UnitId unitId = unit.GetUnitId();
        ServerUnit* unit2 = UnitList::GetUnit(unitId);
        TS_ASSERT_EQUALS(&unit, unit2);
        TS_ASSERT_EQUALS(UnitList::GetCount(), 1);
        UnitList::DeleteUnit(unitId);
        TS_ASSERT_EQUALS(UnitList::GetCount(), 0);
        TS_ASSERT(!UnitList::GetUnit(unitId));
    }

    void TestReuse()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        ServerUnit& unit = UnitList::NewUnit(tile, unitClass);
        UnitList::DeleteUnit(unit.GetUnitId());
        UnitList::NewUnit(tile, unitClass);
        TS_ASSERT_EQUALS(UnitList::GetSize(), 1);
    }

    void TestUnic()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        UnitId unitId = UnitList::NewUnit(tile, unitClass).GetUnitId();
        UnitList::DeleteUnit(unitId);
        ServerUnit& unit = UnitList::NewUnit(tile, unitClass);
        TS_ASSERT_DIFFERS(unitId, unit.GetUnitId());
        TS_ASSERT(!UnitList::GetUnit(unitId));
    }

    void TestIteratorBase()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        UnitList::NewUnit(tile, unitClass);
        UnitList::NewUnit(tile, unitClass);
        UnitList::NewUnit(tile, unitClass);
        TS_ASSERT_EQUALS(UnitList::GetCount(), 3);
        int count = 0;
        for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
        {
            ++count;
            TS_ASSERT(i.GetUnit());
        }
        TS_ASSERT_EQUALS(count, 3);
    }

    void TestIteratorSkip()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        UnitList::NewUnit(tile, unitClass);
        UnitId id = UnitList::NewUnit(tile, unitClass).GetUnitId();
        UnitList::NewUnit(tile, unitClass);
        UnitList::DeleteUnit(id);
        TS_ASSERT_EQUALS(UnitList::GetCount(), 2);
        int count = 0;
        for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
        {
            ++count;
            TS_ASSERT(i.GetUnit());
        }
        TS_ASSERT_EQUALS(count, 2);
    }

    void TestIteratorEmpty()
    {
        UnitList::Clear();
        TS_ASSERT_EQUALS(UnitList::GetCount(), 0);

        int count = 0;
        for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
        {
            ++count;
            TS_ASSERT(i.GetUnit());
        }
        TS_ASSERT_EQUALS(count, 0);
    }

    void TestIteratorEdge()
    {
        UnitList::Clear();
        ServerTile tile(Ogre::Vector3::UNIT_X, 0);
        UnitClass unitClass(0, 0 ,0);

        UnitId id1 = UnitList::NewUnit(tile, unitClass).GetUnitId();
        UnitList::NewUnit(tile, unitClass);
        UnitId id2 = UnitList::NewUnit(tile, unitClass).GetUnitId();
        TS_ASSERT_EQUALS(UnitList::GetCount(), 3);
        UnitList::DeleteUnit(id1);
        UnitList::DeleteUnit(id2);
        TS_ASSERT_EQUALS(UnitList::GetCount(), 1);
        int count = 0;
        for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
        {
            ++count;
            TS_ASSERT(i.GetUnit());
        }
        TS_ASSERT_EQUALS(count, 1);
    }

};

#endif // UNITLISTTEST_H_INCLUDED
