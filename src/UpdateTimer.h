#ifndef UPDATETIMER_H_INCLUDED
#define UPDATETIMER_H_INCLUDED

#include <Typedefs.h>
#include <HighResolutionClock.h>
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
        mStartRWL.lock();
        mStart = GetMiliseconds();
        mStartRWL.unlock();
    }

    int64 GetLeft()
    {
        int64 current = GetMiliseconds();
        mStartRWL.lock_shared();
        int64 passed = current - mStart;
        mStartRWL.unlock_shared();
        int64 left = mPeriod - passed;
        return left < 0 ? 0 : left;
    }

    int64 GetPassedTime() const
    {
        return mPassed;
    }
private:
    const int64 mPeriod;
    int64 mStart;
    int64 mPassed;
    boost::shared_mutex mStartRWL;
};



#endif // UPDATETIMER_H_INCLUDED
