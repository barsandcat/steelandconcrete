#include <pch.h>

#include <Platform.h>

int64 GetMiliseconds()
{
	LARGE_INTEGER stop;
	LARGE_INTEGER proc_freq;
	QueryPerformanceCounter(&stop);
	QueryPerformanceFrequency(&proc_freq);
	return double(stop.QuadPart) / double(proc_freq.QuadPart) * 1000;
}