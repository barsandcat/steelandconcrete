#include <pch.h>
#include <ReadWriteLock.h>

void ReadWriteLock::StartRead()
{
    mReadEntryLock.enter();
    mReadLock.enter();
    mReadCountLock.enter();
    mReadCount++;
    if (mReadCount == 1)
    {
        mWriteLock.enter();
    }
    mReadCountLock.leave();
    mReadLock.leave();
    mReadEntryLock.leave();
}

void ReadWriteLock::StopRead()
{
    mReadCountLock.enter();
    mReadCount--;
    if (mReadCount == 0)
    {
        mWriteLock.leave();
    }
    mReadCountLock.leave();
}

void ReadWriteLock::StartWrite()
{
    mReadLock.enter();
    mWriteLock.enter();
}

void ReadWriteLock::StopWrite()
{
    mWriteLock.leave();
    mReadLock.leave();
}
