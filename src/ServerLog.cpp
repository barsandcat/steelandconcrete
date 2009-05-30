#include <pch.h>
#include <ServerLog.h>

std::ostream& GetLog()
{
    return std::cerr;
}
