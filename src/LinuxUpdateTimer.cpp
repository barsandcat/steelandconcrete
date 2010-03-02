#include <pch.h>

#include <UpdateTimer.h>

int64 UpdateTimer::GetMiliseconds()
{
    timeval epoch;
    gettimeofday(&epoch, NULL);
    return epoch.tv_sec * 1000 + epoch.tv_usec / 1000;
}
