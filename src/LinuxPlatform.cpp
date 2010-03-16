#include <pch.h>

#include <Platform.h>
#include <sys/time.h>

int64 GetMiliseconds()
{
    timeval epoch;
    gettimeofday(&epoch, NULL);
    return epoch.tv_sec * 1000 + epoch.tv_usec / 1000;
}
