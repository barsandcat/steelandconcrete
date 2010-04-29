#ifndef UPDATETIMER_H_INCLUDED
#define UPDATETIMER_H_INCLUDED

#include <Typedefs.h>
#include <Platform.h>
#include <ServerLog.h>
#include <boost/thread.hpp>

class UpdateTimer
{
public:
    UpdateTimer(int64 aPeriod): mPeriod(aPeriod)
    {
        assert(aPeriod > 0);
        mStart = GetMiliseconds();
    }

    void Wait()
    {
        int64 current = GetMiliseconds();
        mPassed = current - mStart;
        int64 left = mPeriod - mPassed;

        if (left > 0)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(left));
        }
        mStart = GetMiliseconds();
    }

    int64 GetPassedTime()
    {
        return mPassed;
    }
private:
    const int64 mPeriod;
    int64 mStart;
    int64 mPassed;
};



#endif // UPDATETIMER_H_INCLUDED
