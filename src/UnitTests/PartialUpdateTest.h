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
        mStranger = &UnitList::NewUnit(*mTiles.at(42), *mUnitClass);
        mNetwork = new DummyNetwork();
        mFOV = new ClientFOV(*mNetwork, mTiles, mUnit->GetUnitId());
    }

    void tearDown()
    {
        UnitList::Clear();
        delete mUnitClass;
        delete mNetwork;
        delete mFOV;
        ServerGeodesicGrid::Tiles::iterator it = mTiles.begin();
        for (;it != mTiles.end(); ++it)
        {
            delete *it;
        }
        mTiles.clear();
    }

    void TestClientFullUpdate()
    {
        mFOV->WriteFullUpdate(1);
        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);

        PayloadMsg showTiles;
        showTiles.set_last(false);

        AddShowTile(showTiles, 0, mTiles);
        AddShowTile(showTiles, 163, mTiles);
        AddShowTile(showTiles, 167, mTiles);
        AddShowTile(showTiles, 171, mTiles);
        AddShowTile(showTiles, 175, mTiles);
        AddShowTile(showTiles, 179, mTiles);

        //std::cout << mNetwork->GetMessages().at(0).DebugString() << std::endl;
        //std::cout << showTiles.DebugString();

        TS_ASSERT(mNetwork->GetMessages().at(0) == showTiles);
    }

    void TestEmptyPartialUpdate()
    {
        mFOV->WriteFullUpdate(1);
        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);

        for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
        {
            (*i)->GetChangeList()->Commit();
        }

        mFOV->WritePartialUpdate(1, 1);
        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);
    }

    void TestMoveInPartialUpdate()
    {
        mFOV->WriteFullUpdate(1);
        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);
        mStranger->Move(*mTiles.at(163));

        for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
        {
            (*i)->GetChangeList()->Commit();
        }

        mFOV->WritePartialUpdate(1, 1);

        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 2);

        PayloadMsg enterMsg;
        ChangeMsg* change = enterMsg.add_changes();
        UnitEnterMsg* msg = change->mutable_unitenter();
        msg->set_unitid(mStranger->GetUnitId());
        msg->set_to(163);
        msg->set_visualcode(0);
        enterMsg.set_last(false);

        TS_ASSERT(mNetwork->GetMessages().at(1) == enterMsg);

        //std::cout << enterMsg.DebugString() << std::endl;

        //std::cout << mNetwork->GetMessages().at(1).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(2).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(3).DebugString() << std::endl;
    }

    void TestClientMovePartialUpdate()
    {
        mFOV->WriteFullUpdate(1);
        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);
        mUnit->Move(*mTiles.at(163));

        for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
        {
            (*i)->GetChangeList()->Commit();
        }

        mFOV->WritePartialUpdate(1, 1);

        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 4);

        PayloadMsg showHideMsg;
        AddShowTile(showHideMsg, 42, mTiles);
        AddShowTile(showHideMsg, 403, mTiles);
        AddShowTile(showHideMsg, 404, mTiles);
        AddHideTile(showHideMsg, 167);
        AddHideTile(showHideMsg, 171);
        showHideMsg.set_last(false);

        TS_ASSERT(mNetwork->GetMessages().at(1) == showHideMsg);

        PayloadMsg enterMsg;
        ChangeMsg* change = enterMsg.add_changes();
        UnitEnterMsg* msg = change->mutable_unitenter();
        msg->set_unitid(mUnit->GetUnitId());
        msg->set_to(163);
        enterMsg.set_last(false);

        TS_ASSERT(mNetwork->GetMessages().at(3) == enterMsg);

        //std::cout << showHideMsg.DebugString() << std::endl;

        //std::cout << mNetwork->GetMessages().at(1).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(2).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(3).DebugString() << std::endl;
    }

    void TestLeavePartialUpdate()
    {
        mStranger->Move(*mTiles.at(163));

        for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
        {
            (*i)->GetChangeList()->Commit();
        }

        mFOV->WriteFullUpdate(1);

        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 1);

        mStranger->Move(*mTiles.at(42));

        for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
        {
            (*i)->GetChangeList()->Commit();
        }

        mFOV->WritePartialUpdate(1, 1);

        TS_ASSERT_EQUALS(mNetwork->GetMessages().size(), 2);

        PayloadMsg leaveMsg;
        ChangeMsg* change = leaveMsg.add_changes();
        UnitLeaveMsg* msg = change->mutable_unitleave();
        msg->set_unitid(mStranger->GetUnitId());
        msg->set_to(42);
        leaveMsg.set_last(false);

        TS_ASSERT(mNetwork->GetMessages().at(1) == leaveMsg);

        //std::cout << enterMsg.DebugString() << std::endl;

        //std::cout << mNetwork->GetMessages().at(1).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(2).DebugString() << std::endl;
        //std::cout << mNetwork->GetMessages().at(3).DebugString() << std::endl;
    }

private:
    UnitClass* mUnitClass;
    ServerUnit* mUnit;
    ServerUnit* mStranger;
    DummyNetwork* mNetwork;
    ClientFOV* mFOV;
    ServerGeodesicGrid::Tiles mTiles;
};



#endif // PARTIALUPDATETEST_H_INCLUDED
