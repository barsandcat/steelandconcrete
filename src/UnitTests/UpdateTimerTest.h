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
        TS_ASSERT_EQUALS(0, timer.GetUpdateTime());
        timer.Wait();
        TS_ASSERT_LESS_THAN_EQUALS(period, timer.GetUpdateTime());
        timer.Wait();
        TS_ASSERT_LESS_THAN_EQUALS(period, timer.GetUpdateTime());
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
