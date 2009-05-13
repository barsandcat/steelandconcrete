//-< WTASK.CXX >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      5-Apr-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jun-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Tasking implemented for Win32
//-------------------------------------------------------------------*--------*

#include "task.h"
#include "console.h"
//
// Task 
//

task* task::create(fptr f, void* arg, priority pri, size_t stack_size)
{ 
    DWORD threadid;
    HANDLE h = CreateThread(NULL, stack_size, 
			    LPTHREAD_START_ROUTINE(f), arg,
			    CREATE_SUSPENDED, &threadid);
    if (h == NULL) { 
        console::error("CreateThread failed with erro code=%d\n", GetLastError());
        return NULL;
    }
    SetThreadPriority(h, THREAD_PRIORITY_LOWEST + 
		(THREAD_PRIORITY_HIGHEST - THREAD_PRIORITY_LOWEST) 
		* (pri - pri_background) 
		/ (pri_realtime - pri_background));
    ResumeThread(h);
    CloseHandle(h);
    return (task*)threadid; 
}
 
void task::initialize(size_t) {}

void task::reschedule() {}

void task::reschedule(priority) {}

task* task::current() 
{ 
    return (task*)GetCurrentThreadId();
}

void task::exit() 
{
    ExitThread(0);
} 



void task::sleep(timeout_t msec)
{
    Sleep(msec);
}


