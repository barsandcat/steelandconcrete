#ifndef SYNCTIMER_H
#define SYNCTIMER_H

#include <Typedefs.h>
#include <HighResolutionClock.h>

class SyncTimer
{
public:
    SyncTimer(int64 aPeriod) { Reset(aPeriod); }
    void Reset(int64 aPeriod)
    {
        mEnd = aPeriod + GetMiliseconds();
    }
    bool IsTime()
    {
        return GetMiliseconds() >= mEnd;
    }
    ~SyncTimer() {}
private:
    SyncTimer(const SyncTimer& other) {}
    SyncTimer& operator=(const SyncTimer& other)
    {
        return *this;
    }
private:
    int64 mEnd;
};

#endif // SYNCTIMER_H
