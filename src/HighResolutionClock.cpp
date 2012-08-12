#include <pch.h>
#include <HighResolutionClock.h>

#include <boost/chrono/chrono.hpp>

int64 GetMiliseconds()
{
    boost::chrono::high_resolution_clock::time_point now = boost::chrono::high_resolution_clock::now();
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(now.time_since_epoch()).count();
}
