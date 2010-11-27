#include <pch.h>
#include <ServerLog.h>

std::ostream& GetLog()
{
    struct tm *pTime;
    time_t ctTime; time(&ctTime);
    pTime = localtime(&ctTime);
    std::cerr << std::endl << std::setw(2) << std::setfill('0') << pTime->tm_hour
    << ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
    << ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
    << ": ";
    return std::cerr;
}
