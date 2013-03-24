#ifndef COMPAREPAYLOADTEST_H_INCLUDED
#define COMPAREPAYLOADTEST_H_INCLUDED


// MyTestSuite.h
#include <cxxtest/TestSuite.h>

#include <ComparePayload.h>

class ComparePayloadTest : public CxxTest::TestSuite
{
public:
    void setUp()
    {
        msgLeave.set_last(true);
        msgLeave.set_time(9999);
        msgLeave.set_reason("Reason");
        ChangeMsg* change = msgLeave.add_changes();
        UnitLeaveMsg* leave = change->mutable_unitleave();
        leave->set_unitid(1);
        leave->set_to(2);

        msgEnter.set_last(true);
        msgEnter.set_time(9999);
        msgEnter.set_reason("Reason");
        change = msgEnter.add_changes();
        UnitEnterMsg* enter = change->mutable_unitenter();
        enter->set_to(2);
        enter->set_unitid(3);
        enter->set_visualcode(8);
    }

    void tearDown()
    {
        msgLeave.Clear();
        msgEnter.Clear();
    }

    void TestEqualEmpty()
    {
        PayloadMsg msg1;
        PayloadMsg msg2;
        TS_ASSERT(msg1 == msg2);
    }

    void TestSelfEqual()
    {
        TS_ASSERT(msgLeave == msgLeave);
        TS_ASSERT(msgEnter == msgEnter);
    }

    void TestNotEqual()
    {
        PayloadMsg msg1;
        TS_ASSERT(!(msgLeave == msgEnter));
        TS_ASSERT(!(msg1 == msgEnter));
    }

private:
    PayloadMsg msgLeave;
    PayloadMsg msgEnter;

};



#endif // COMPAREPAYLOADTEST_H_INCLUDED
