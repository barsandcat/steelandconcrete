#include <pch.h>

#include <Platform.h>
#include <sys/time.h>

extern const char* RU_LOCALE = "ru_RU";
extern const char* UK_LOCALE = "uk_UA";
extern const char* EN_LOCALE = "en_US";
extern const char* JA_LOCALE = "ja_JP";

int64 GetMiliseconds()
{
    timeval epoch;
    gettimeofday(&epoch, NULL);
    return epoch.tv_sec * 1000 + epoch.tv_usec / 1000;
}
