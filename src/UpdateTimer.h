#ifndef UPDATETIMER_H_INCLUDED
#define UPDATETIMER_H_INCLUDED

#include <Typedefs.h>
#include <task.h>
#include <sys/time.h>

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
        mStart = current;
        int64 left = mPeriod - mPassed;
        if (left > 0)
        {
            task::sleep(left);
        }
    }

    int64 GetUpdateTime()
    {
        return mPassed;
    }

private:
    int64 GetMiliseconds();
private:
    const int64 mPeriod;
    int64 mStart;
    int64 mPassed;
};



#endif // UPDATETIMER_H_INCLUDED
