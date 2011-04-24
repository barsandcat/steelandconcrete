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
    }

    void tearDown()
    {
        delete mNetwork;
        delete mChangeList;
    }

    void TestChangeListOneBlock()
    {
        const int count = 50;
        for (int i = 0; i < count; ++i)
        {
            mChangeList->AddRemove(i);
        }
        mChangeList->Write(*mNetwork, 0);

        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), count);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 1);
    }

    void TestChangeListTwoTimes()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit();
        mChangeList->AddRemove(1);
        mChangeList->Commit();
        mChangeList->AddRemove(2);
        mChangeList->Write(*mNetwork, 0);


        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 1);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 1);
    }

    void TestChangeListClientWrong()
    {
        mChangeList->AddRemove(0);
        mChangeList->Commit();
        mChangeList->AddRemove(1);
        TS_ASSERT_THROWS_ANYTHING(mChangeList->Write(*mNetwork, 2));


        TS_ASSERT_EQUALS(mNetwork->GetChangesWrited(), 0);
        TS_ASSERT_EQUALS(mNetwork->GetWrites(), 0);
    }

private:
    DummyNetwork* mNetwork;
    ChangeList* mChangeList;
};


#endif
