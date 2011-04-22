#ifndef NETWORKTEST_H_INCLUDED
#define NETWORKTEST_H_INCLUDED

// MyTestSuite.h
#include <cxxtest/TestSuite.h>
#include <proto/Tile.pb.h>
#include <proto/Vector.pb.h>

#include <iostream>
#include <cstdlib>
#include <ChangeList.h>
#include <DummyNetwork.h>
#include <Exceptions.h>

class MyTestSuite : public CxxTest::TestSuite
{
public:
    void setUp()
    {
        mNetwork = new DummyNetwork();
        mChangeList = new ChangeList();
        mChangeList->Commit(0);
        mChangeList->Clear();
    }

    void tearDown()
    {
        delete mNetwork;
        delete mChangeList;
    }

    void TestChangeListOneBlock()
    {
        const GameTime time = 99;
        const int count = 50;
        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Commit(time);
        mChangeList->Write(*mNetwork, time - 1, 1000);
        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 2);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), time);
    }

    void TestChangeListTwoBlock()
    {
        const GameTime time = 20000;
        const int count = 400;
        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Commit(time);
        mChangeList->Write(*mNetwork, time - 1, 1000);
        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 3);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), time);
    }

    void TestChangeListTwoTimes()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit(0);
        mChangeList->AddRemove(1);
        mChangeList->Commit(1);
        mChangeList->AddRemove(2);
        mChangeList->Commit(2);
        mChangeList->Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 2);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 3);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(2));
    }

    void TestChangeListUsualCase()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit(0);
        mChangeList->AddRemove(1);
        mChangeList->Commit(1);
        mChangeList->Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 2);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(1));
    }

    void TestChangeListClientWrong()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit(0);
        mChangeList->AddRemove(1);
        mChangeList->Commit(1);
        mChangeList->Write(*mNetwork, 2, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(1));
    }

    void TestChangeListClientLate()
    {
        mChangeList->AddRemove(10);
        mChangeList->Commit(10);
        mChangeList->AddRemove(11);
        mChangeList->Commit(11);
        mChangeList->AddRemove(12);
        mChangeList->Commit(12);

        TS_ASSERT_THROWS(mChangeList->Write(*mNetwork, 2, 1000), ClientBehind);

        TS_ASSERT(!mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 0);
    }

    void TestChangeListClientSameTime()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit(0);
        mChangeList->AddRemove(1);
        mChangeList->Commit(1);
        mChangeList->Write(*mNetwork, 1, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(1));
    }

    void TestChangeListTwoTimesTwoBlocks()
    {
        const int count = 400;


        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Commit(0);

        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Commit(1);

        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Commit(2);

        mChangeList->Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count * 2);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 5);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(2));
    }

private:
    DummyNetwork* mNetwork;
    ChangeList* mChangeList;

};


#endif
