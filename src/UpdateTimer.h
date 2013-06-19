#ifndef UPDATETIMER_H_INCLUDED
#define UPDATETIMER_H_INCLUDED

#include <Typedefs.h>
#include <HighResolutionClock.h>
#include <boost/thread.hpp>

class UpdateTimer
{
public:
    UpdateTimer(Miliseconds aPeriod): mPeriod(aPeriod)
    {
        assert(aPeriod > 0);
        mStart = GetMiliseconds();
    }

    void Wait()
    {
        Miliseconds current = GetMiliseconds();
        mPassed = current - mStart;
        Miliseconds left = mPeriod - mPassed;

        if (left > 0)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(left));
        }
        mStartRWL.lock();
        mStart = GetMiliseconds();
        mStartRWL.unlock();
    }

    Miliseconds GetLeft()
    {
        Miliseconds current = GetMiliseconds();
        mStartRWL.lock_shared();
        Miliseconds passed = current - mStart;
        mStartRWL.unlock_shared();
        Miliseconds left = mPeriod - passed;
        return left < 0 ? 0 : left;
    }

    Miliseconds GetPassedTime() const
    {
        return mPassed;
    }
private:
    const Miliseconds mPeriod;
    Miliseconds mStart;
    Miliseconds mPassed;
    boost::shared_mutex mStartRWL;
};



#endif // UPDATETIMER_H_INCLUDED
