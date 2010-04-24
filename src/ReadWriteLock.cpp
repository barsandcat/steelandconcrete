#include <pch.h>
#include <ReadWriteLock.h>

ReadWriteLock::ReadWriteLock(): mReadCount(0), mWriteEvent(true)
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
        mWriteEvent.reset();
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
        mWriteEvent.signal();
    }
    mReadCountLock.leave();
}

void ReadWriteLock::StartWrite()
{
    mReadLock.enter();
    mWriteEvent.wait();
}

void ReadWriteLock::StopWrite()
{
    mReadLock.leave();
}
