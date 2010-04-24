#ifndef UPDATETIMERTEST_H_INCLUDED
#define UPDATETIMERTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <UpdateTimer.h>
#include <SyncTimer.h>

class UpdateTimerTest: public CxxTest::TestSuite
{
public:
    void TestTimer()
    {
        int64 period = 100;
        UpdateTimer timer(period);
        timer.Wait();
        TS_ASSERT_EQUALS(0, timer.GetPassedTime());

        task::sleep(50);
        timer.Wait();
        TS_ASSERT_DIFFERS(timer.GetPassedTime(), 0);

        task::sleep(100);
        timer.Wait();
        TS_ASSERT_DIFFERS(timer.GetPassedTime(), 0);
    }

    void TestSyncTimer()
    {
        SyncTimer timer(100);
        TS_ASSERT(!timer.IsTime());
        task::sleep(100);
        TS_ASSERT(timer.IsTime());
    }

};


#endif // UPDATETIMERTEST_H_INCLUDED
