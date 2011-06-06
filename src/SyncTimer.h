#ifndef SYNCTIMER_H
#define SYNCTIMER_H

#include <Typedefs.h>
#include <Platform.h>

class SyncTimer
{
public:
    SyncTimer(int64 aPeriod) { Reset(aPeriod); }
    void Reset(int64 aPeriod)
    {
        mEnd = aPeriod + GetMiliseconds();
        mActive = true;
    }
    bool IsTime()
    {
        bool isTime = GetMiliseconds() >= mEnd;
        bool result = mActive && isTime;
        mActive = mActive && !isTime;
        return result;
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
    bool mActive;
};

#endif // SYNCTIMER_H
