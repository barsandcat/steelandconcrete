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
        ChangeList::SetTime(0);
        mClass = new UnitClass(0, 100, 1);
        mTile = new ServerTile(Ogre::Vector3::UNIT_X, 5000);
        mUnit = new ServerUnit(*mTile, *mClass, 0);
    }

    void tearDown()
    {
        delete mUnit;
        delete mTile;
        delete mClass;
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
