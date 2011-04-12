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
        ChangeList::Commit(0);
        ChangeList::Clear();
        mNetwork = new DummyNetwork();
    }

    void tearDown()
    {
        delete mNetwork;
    }

    void TestChangeListOneBlock()
    {
        const GameTime time = 99;
        const int count = 50;
        for (int i = 0; i < count; ++i)
        {
            ChangeList::AddRemove(i);
        }
        ChangeList::Commit(time);
        ChangeList::Write(*mNetwork, time - 1, 1000);
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
            ChangeList::AddRemove(i);
        }
        ChangeList::Commit(time);
        ChangeList::Write(*mNetwork, time - 1, 1000);
        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 3);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), time);
    }

    void TestChangeListTwoTimes()
    {
        ChangeList::AddRemove(0);
        ChangeList::Commit(0);
        ChangeList::AddRemove(1);
        ChangeList::Commit(1);
        ChangeList::AddRemove(2);
        ChangeList::Commit(2);
        ChangeList::Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 2);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 3);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(2));
    }

    void TestChangeListUsualCase()
    {
        ChangeList::AddRemove(0);
        ChangeList::Commit(0);
        ChangeList::AddRemove(1);
        ChangeList::Commit(1);
        ChangeList::Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 2);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(1));
    }

    void TestChangeListClientWrong()
    {
        ChangeList::AddRemove(0);
        ChangeList::Commit(0);
        ChangeList::AddRemove(1);
        ChangeList::Commit(1);
        ChangeList::Write(*mNetwork, 2, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(1));
    }

    void TestChangeListClientLate()
    {
        ChangeList::AddRemove(10);
        ChangeList::Commit(10);
        ChangeList::AddRemove(11);
        ChangeList::Commit(11);
        ChangeList::AddRemove(12);
        ChangeList::Commit(12);

        TS_ASSERT_THROWS(ChangeList::Write(*mNetwork, 2, 1000), ClientBehind);

        TS_ASSERT(!mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 0);
    }

    void TestChangeListClientSameTime()
    {
        ChangeList::AddRemove(0);
        ChangeList::Commit(0);
        ChangeList::AddRemove(1);
        ChangeList::Commit(1);
        ChangeList::Write(*mNetwork, 1, 1000);

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
            ChangeList::AddRemove(i);
        }
        ChangeList::Commit(0);

        for (int i = 0; i < count; ++i)
        {
            ChangeList::AddRemove(i);
        }
        ChangeList::Commit(1);

        for (int i = 0; i < count; ++i)
        {
            ChangeList::AddRemove(i);
        }
        ChangeList::Commit(2);

        ChangeList::Write(*mNetwork, 0, 1000);

        TS_ASSERT(mNetwork->IsLastWrited());
        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count * 2);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 5);
        TS_ASSERT_EQUALS(mNetwork->GetTimeWrited(), GameTime(2));
    }

private:
    DummyNetwork* mNetwork;

};


#endif
