#ifndef UPDATETIMERTEST_H_INCLUDED
#define UPDATETIMERTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <UpdateTimer.h>
#pragma GCC diagnostic pop
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

        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        timer.Wait();
        TS_ASSERT_DIFFERS(timer.GetPassedTime(), 0);

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        timer.Wait();
        TS_ASSERT_DIFFERS(timer.GetPassedTime(), 0);
    }

    void TestSyncTimer()
    {
        SyncTimer timer(90);
        TS_ASSERT(!timer.IsTime());
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        TS_ASSERT(timer.IsTime());
        TS_ASSERT(timer.IsTime());
        timer.Reset(0);
        TS_ASSERT(timer.IsTime());
    }

};


#endif // UPDATETIMERTEST_H_INCLUDED
