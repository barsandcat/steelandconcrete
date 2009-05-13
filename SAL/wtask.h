//-< WTASK.H >-------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      5-Apr-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 23-Sep-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Tasking implementation for Win32
//-------------------------------------------------------------------*--------*

#ifndef __WTASK_H__
#define __WTASK_H__

#define task_proc WINAPI

#define MAX_SEM_VALUE 1000000

class mutex_internals { 
    friend class eventex_internals;
    friend class semaphorex_internals;
  protected: 
    CRITICAL_SECTION cs;

    void enter() { EnterCriticalSection(&cs); }
    void leave() { LeaveCriticalSection(&cs); }

    mutex_internals() { InitializeCriticalSection(&cs); }
    ~mutex_internals() { DeleteCriticalSection(&cs); } 
};

class semaphore_internals { 
  protected: 
    HANDLE h;
    
    void wait() { WaitForSingleObject(h, INFINITE); }
    boolean wait_with_timeout(timeout_t msec) { 
	return WaitForSingleObject(h, msec) == WAIT_OBJECT_0;
    }
    void signal() { ReleaseSemaphore(h, 1, NULL); }

    semaphore_internals(int init_count) { 
	h = CreateSemaphore(NULL, init_count, MAX_SEM_VALUE, NULL); 
    }
    ~semaphore_internals() { CloseHandle(h); }
};

class semaphorex_internals { 
  protected: 
    HANDLE h;
    CRITICAL_SECTION* cs;

    void wait() { 
	LeaveCriticalSection(cs);
	WaitForSingleObject(h, INFINITE); 
	EnterCriticalSection(cs);
    }
    boolean wait_with_timeout(timeout_t msec) { 
	LeaveCriticalSection(cs);
	int result = WaitForSingleObject(h, msec);
	EnterCriticalSection(cs);
	return result == WAIT_OBJECT_0;
    }
    void signal() { ReleaseSemaphore(h, 1, NULL); }

    semaphorex_internals(mutex_internals& guard, int init_count) { 
	cs = &guard.cs;
	h = CreateSemaphore(NULL, init_count, MAX_SEM_VALUE, NULL); 
    }
    ~semaphorex_internals() { CloseHandle(h); }
};

class event_internals { 
  protected: 
    HANDLE h;

    void wait() { WaitForSingleObject(h, INFINITE); }

    boolean wait_with_timeout(timeout_t msec) { 
	return WaitForSingleObject(h, msec) == WAIT_OBJECT_0;
    }

    void signal() { SetEvent(h); }

    void reset() { ResetEvent(h); }

    event_internals(boolean signaled) { 
	h = CreateEvent(NULL, True, signaled, NULL); 
    }
    ~event_internals() { CloseHandle(h); }
}; 

class eventex_internals { 
  protected: 
    HANDLE  h;
    HANDLE  s;
    int     wait_queue_len;
    boolean signaled;
    CRITICAL_SECTION* cs;

    void wait() { 
	if (!signaled) { 
	    wait_queue_len += 1;
	    LeaveCriticalSection(cs);
	    WaitForSingleObject(h, INFINITE);
	    ReleaseSemaphore(s, 1, NULL);
	    EnterCriticalSection(cs);
	}
    }

    boolean wait_with_timeout(timeout_t msec) { 
	if (!signaled) { 
	    wait_queue_len += 1;
	    LeaveCriticalSection(cs);
	    int result =  WaitForSingleObject(h, msec);
	    ReleaseSemaphore(s, 1, NULL);
	    EnterCriticalSection(cs);
	    return result == WAIT_OBJECT_0;
	}
	return True;
    }
	
    void signal() { // should be called with cs mutex locked
	SetEvent(h); 
	signaled = True;
	while (wait_queue_len != 0) { 
	    WaitForSingleObject(s, INFINITE);
	    wait_queue_len -= 1;
	} 
    }

    void reset() { // should be called with cs mutex locked
	signaled = False;
	ResetEvent(h); 
    }

    eventex_internals(mutex_internals& guard, boolean signaled)  { 
	wait_queue_len = 0;
	this->signaled = signaled;
	cs = &guard.cs;
	h = CreateEvent(NULL, True, False, NULL); 
	s = CreateSemaphore(NULL, 0, MAX_SEM_VALUE, NULL); 
    }
    ~eventex_internals() { 
	CloseHandle(h); 
	CloseHandle(s); 
    }
}; 

class task_internals { };

#endif

