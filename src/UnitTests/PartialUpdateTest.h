#ifndef PARTIALUPDATETEST_H_INCLUDED
#define PARTIALUPDATETEST_H_INCLUDED


#include <cxxtest/TestSuite.h>
#include <ServerGeodesicGrid.h>
#include <UnitList.h>
#include <DummyNetwork.h>
#include <ClientFOV.h>
#include <ComparePayload.h>

class PartialUpdateTest: public CxxTest::TestSuite
{
public:
    void setUp()
    {
        ServerGeodesicGrid grid(mTiles, 2);
        mUnitClass = new UnitClass(0, 0, 0);
        mUnit = &UnitList::NewUnit(*mTiles.at(0), *mUnitClass);
        mNetwork = new DummyNetwork();
        mFOV = new ClientFOV(*mNetwork, mTiles, mUnit->GetUnitId());
    }

    void tearDown()
    {
        UnitList::Clear();
        delete mUnitClass;
        delete mNetwork;
        delete mFOV;
    }

    void TestA()
    {
        mFOV->SendUpdate(1, 0, 1, 1, 1);
        TS_ASSERT(mNetwork->GetMessages().size() == 2);
        PayloadMsg showTiles;
        TS_ASSERT(mNetwork->GetMessages().at(0) == showTiles);
        PayloadMsg fin;
        TS_ASSERT(mNetwork->GetMessages().at(1) == fin);
    }

private:
    UnitClass* mUnitClass;
    ServerUnit* mUnit;
    DummyNetwork* mNetwork;
    ClientFOV* mFOV;
    ServerGeodesicGrid::Tiles mTiles;
};



#endif // PARTIALUPDATETEST_H_INCLUDED
