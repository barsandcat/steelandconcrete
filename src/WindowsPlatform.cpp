#include <pch.h>

#include <Platform.h>

extern const char* RU_LOCALE = "Russian_Russia";
extern const char* UK_LOCALE = "Ukrainian_Ukraine";
extern const char* EN_LOCALE = "English_United States";
extern const char* JA_LOCALE = "Japanese_Japan";

int64 GetMiliseconds()
{
	LARGE_INTEGER stop;
	LARGE_INTEGER proc_freq;
	QueryPerformanceCounter(&stop);
	QueryPerformanceFrequency(&proc_freq);
	return double(stop.QuadPart) / double(proc_freq.QuadPart) * 1000;
}