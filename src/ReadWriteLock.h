#ifndef READWRITELOCK_H
#define READWRITELOCK_H

#include <task.h>

// Communications of the ACM:Concurrent Control with "Readers" and "Writers"
// P.J. Courtois,* F. H, 1971
// Simplified solution for one writer only
class ReadWriteLock
{
public:
    ReadWriteLock();
    ~ReadWriteLock();
    void StartRead();
    void StopRead();
    void StartWrite();
    void StopWrite();
protected:
private:
    ReadWriteLock(const ReadWriteLock& other) {}
    ReadWriteLock& operator=(const ReadWriteLock& other) { return *this; }

    int mReadCount; // enshures that only one reader manipulates mWriteLock
    simple_mutex mReadCountLock; // protects counter
    simple_mutex mReadEntryLock; // prvents readers retaking mReadLock, if writer is trying to do so
    simple_mutex mWriteLock;
    simple_mutex mReadLock;
};

#endif // READWRITELOCK_H