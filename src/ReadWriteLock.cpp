#include <pch.h>
#include <ReadWriteLock.h>

ReadWriteLock::ReadWriteLock(): mReadCount(0)
{
}

ReadWriteLock::~ReadWriteLock()
{
}

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
