#ifndef SERVERUNITTEST_H_INCLUDED
#define SERVERUNITTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <Ogre.h>
#include <ChangeList.h>

class ServerUnitTest: public CxxTest::TestSuite
{
public:
    void setUp()
    {
        mClass = new UnitClass(0, 100, 1);
        mTile = new ServerTile(0, Ogre::Vector3::UNIT_X);
        mUnit = new ServerUnit(*mTile, *mClass, 0);
    }

    void tearDown()
    {
        delete mUnit;
        delete mTile;
        delete mClass;
    }

    void TestDelete()
    {
        delete mUnit;
        mUnit = NULL;
        TS_ASSERT_EQUALS(mTile->GetUnitId(), 0);
    }

    void TestAgeZero()
    {
        TS_ASSERT(!mUnit->UpdateAgeAndIsTimeToDie(0));
    }

    void TestAgeThousend()
    {
        TS_ASSERT(mUnit->UpdateAgeAndIsTimeToDie(1000));
    }
private:
    ServerUnit* mUnit;
    ServerTile* mTile;
    UnitClass* mClass;
};

#endif
