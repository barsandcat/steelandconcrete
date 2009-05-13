//-< PTASK.CXX >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      2-Mar-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 14-Sep-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Mulitasking implemented using Posix pthreads 
//-------------------------------------------------------------------*--------*

#include "task.h"
#include "console.h"
#include <errno.h>
#include <sys/time.h>

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 8192
#endif

#if !defined(PRI_OTHER_MAX)
#if defined(__posix4__)
#include <sched.h>

int PRI_OTHER_MIN;
int PRI_OTHER_MAX;
#else
#define PRI_OTHER_MIN 1
#define PRI_OTHER_MAX 32
#endif
#endif

#define SYSCHECK(op) if ((op) < 0) console::error(#op ":%d failed with errno = %d\n", __LINE__, errno)


//
// Sempahore
//

semaphore_internals::semaphore_internals(int init_count) 
{ 
    SYSCHECK(pthread_mutex_init(&cs, NULL));
    SYSCHECK(pthread_cond_init(&cond, NULL));
    count = init_count;
}

semaphore_internals::~semaphore_internals() 
{ 
    pthread_mutex_destroy(&cs);
    pthread_cond_destroy(&cond);
}

void semaphore_internals::wait() 
{ 
    SYSCHECK(pthread_mutex_lock(&cs));
    while (count <= 0) { 
	SYSCHECK(pthread_cond_wait(&cond, &cs));
    } 
    count -= 1;
    SYSCHECK(pthread_mutex_unlock(&cs));
} 

boolean semaphore_internals::wait_with_timeout(timeout_t msec) 
{
    int rc; 
    SYSCHECK(pthread_mutex_lock(&cs));
    while (count <= 0) { 
	struct timespec rel_ts, abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
	rel_ts.tv_sec = msec/1000; 
        rel_ts.tv_nsec = msec%1000*1000000;
	pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
	struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
	abs_ts.tv_sec = cur_tv.tv_sec + msec/1000; 
	abs_ts.tv_nsec = (cur_tv.tv_usec + msec%1000*1000)*1000;
#endif
	int rc = pthread_cond_timedwait(&cond, &cs, &abs_ts);
	if (rc == ETIMEDOUT) { 
	    pthread_mutex_unlock(&cs);
	    return False;
	}
	SYSCHECK(rc);
    } 
    count -= 1;
    SYSCHECK(pthread_mutex_unlock(&cs));
    return True;
} 

void semaphore_internals::signal() 
{ 
    SYSCHECK(pthread_mutex_lock(&cs));
    count += 1; 
    SYSCHECK(pthread_cond_signal(&cond));
    SYSCHECK(pthread_mutex_unlock(&cs));
}

//
// Sempahore with associated mutex
//

semaphorex_internals::semaphorex_internals(mutex_internals& cs, int init_count)
: guard(cs)
{ 
    SYSCHECK(pthread_cond_init(&cond, NULL));
    count = init_count;
}

semaphorex_internals::~semaphorex_internals() 
{ 
    pthread_cond_destroy(&cond);
}

void semaphorex_internals::wait() 
{ 
#ifndef PTHREAD_MUTEX_RECURSIVE_NP
    pthread_t self = guard.owner; 
    assert(self == pthread_self() && guard.count == 1);
    guard.count = 0;
    guard.owner = 0; 
#endif
    while (count <= 0) { 
	SYSCHECK(pthread_cond_wait(&cond, &guard.cs));
    } 
    count -= 1;
#ifndef PTHREAD_MUTEX_RECURSIVE_NP
    guard.count = 1;
    guard.owner = self;
#endif
} 

boolean semaphorex_internals::wait_with_timeout(timeout_t msec) 
{
    int rc; 
#ifndef PTHREAD_MUTEX_RECURSIVE_NP
    pthread_t self = guard.owner; 
    assert(self == pthread_self() && guard.count == 1);
#endif
    while (count <= 0) { 
	struct timespec rel_ts, abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
	rel_ts.tv_sec = msec/1000; 
        rel_ts.tv_nsec = msec%1000*1000000;
	pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
	struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
	abs_ts.tv_sec = cur_tv.tv_sec + msec/1000; 
	abs_ts.tv_nsec = (cur_tv.tv_usec + msec%1000*1000)*1000;
#endif
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
	int rc = pthread_cond_timedwait(&cond, &guard.cs, &abs_ts);
#else
	guard.count = 0;
	guard.owner = 0; 
	int rc = pthread_cond_timedwait(&cond, &guard.cs, &abs_ts);
	guard.count = 1;
	guard.owner = self; 
#endif
	if (rc == ETIMEDOUT) { 
	    return False;
	}
	SYSCHECK(rc);
    } 
    count -= 1;
    return True;
} 

void semaphorex_internals::signal() 
{ 
    count += 1; 
    SYSCHECK(pthread_cond_signal(&cond));
}

//
// Event
// 

event_internals::event_internals(boolean signaled) 
{ 
    SYSCHECK(pthread_mutex_init(&cs, NULL));
    SYSCHECK(pthread_cond_init(&cond, NULL));
    this->signaled = signaled;
}

event_internals::~event_internals() 
{ 
    pthread_mutex_destroy(&cs);
    pthread_cond_destroy(&cond);
}

void event_internals::signal()
{
    SYSCHECK(pthread_mutex_lock(&cs));
    signaled = True;
    n_signals += 1;
    SYSCHECK(pthread_cond_broadcast(&cond));
    SYSCHECK(pthread_mutex_unlock(&cs));
}

void event_internals::wait() 
{
    SYSCHECK(pthread_mutex_lock(&cs));
    long before_n_signals = n_signals;
    while (!signaled && n_signals == before_n_signals) { 
	SYSCHECK(pthread_cond_wait(&cond, &cs));
    }
    SYSCHECK(pthread_mutex_unlock(&cs));
}

boolean event_internals::wait_with_timeout(timeout_t msec) 
{
    int rc; 
    SYSCHECK(pthread_mutex_lock(&cs));
    long before_n_signals = n_signals;
    while (!signaled && n_signals == before_n_signals) { 
	struct timespec rel_ts, abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
	rel_ts.tv_sec = msec/1000; 
        rel_ts.tv_nsec = msec%1000*1000000;
	pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
	struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
	abs_ts.tv_sec = cur_tv.tv_sec + msec/1000; 
	abs_ts.tv_nsec = (cur_tv.tv_usec + msec%1000*1000)*1000;
#endif
	int rc = pthread_cond_timedwait(&cond, &cs, &abs_ts);
	if (rc == ETIMEDOUT) { 
	    pthread_mutex_unlock(&cs);
	    return False;
	}
	SYSCHECK(rc);
    } 
    SYSCHECK(pthread_mutex_unlock(&cs));
    return True;
} 

//
// Event with associated mutex
// 

eventex_internals::eventex_internals(mutex_internals& cs, boolean signaled)
: guard(cs) 
{ 
    SYSCHECK(pthread_cond_init(&cond, NULL));
    this->signaled = signaled;
}

eventex_internals::~eventex_internals() 
{ 
    pthread_cond_destroy(&cond);
}

void eventex_internals::signal()
{
    signaled = True;
    n_signals += 1;
    SYSCHECK(pthread_cond_broadcast(&cond));
}

void eventex_internals::wait() 
{
    long before_n_signals = n_signals;
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
    while (!signaled && n_signals == before_n_signals) { 
	SYSCHECK(pthread_cond_wait(&cond, &guard.cs));
    }
#else
    pthread_t self = guard.owner; 
    assert(self == pthread_self() && guard.count == 1);
    guard.count = 0;
    guard.owner = 0; 
    while (!signaled && n_signals == before_n_signals) { 
	SYSCHECK(pthread_cond_wait(&cond, &guard.cs));
    }
    guard.count = 1;
    guard.owner = self;
#endif
}

boolean eventex_internals::wait_with_timeout(timeout_t msec) 
{
    int rc; 
    long before_n_signals = n_signals;
#ifndef PTHREAD_MUTEX_RECURSIVE_NP
    pthread_t self = guard.owner; 
    assert(self == pthread_self() && guard.count == 1);
#endif
    while (!signaled && n_signals == before_n_signals) { 
	struct timespec rel_ts, abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
	rel_ts.tv_sec = msec/1000; 
        rel_ts.tv_nsec = msec%1000*1000000;
	pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
	struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
	abs_ts.tv_sec = cur_tv.tv_sec + msec/1000; 
	abs_ts.tv_nsec = (cur_tv.tv_usec + msec%1000*1000)*1000;
#endif
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
	int rc = pthread_cond_timedwait(&cond, &guard.cs, &abs_ts);
#else
	guard.count = 0;
	guard.owner = 0; 
	int rc = pthread_cond_timedwait(&cond, &guard.cs, &abs_ts);
	guard.count = 1;
	guard.owner = self; 
#endif
	if (rc == ETIMEDOUT) { 
	    return False;
	}
	SYSCHECK(rc);
    } 
    return True;
} 

//
// Mutex
//

mutex_internals::mutex_internals() 
{ 
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
    static boolean initialized = False; 
    static pthread_mutexattr_t recursive_mutexattr; 
    if (!initialized) { 
	SYSCHECK(pthread_mutexattr_init(&recursive_mutexattr));
	SYSCHECK(pthread_mutexattr_settype_np(&recursive_mutexattr, 
					      PTHREAD_MUTEX_RECURSIVE_NP));
	initialized = True;
    }
    SYSCHECK(pthread_mutex_init(&cs, &recursive_mutexattr));
#else
    count = 0;
    owner = 0;
    SYSCHECK(pthread_mutex_init(&cs, NULL));
#endif
}

mutex_internals::~mutex_internals() 
{ 
    pthread_mutex_destroy(&cs);
}

//
// Task 
//

pthread_key_t task_internals::thread_key; 

void* task_internals::create_thread(void* arg) 
{
    SYSCHECK(pthread_setspecific(thread_key, arg));
    task* t = (task*)arg; 
#ifndef PTHREAD_CREATE_DETACHED
    // pthread_detach(t->thread); - bug in RedHat 9.0
    pthread_detach(pthread_self()); 
#endif  
    (*t->f)(t->arg); 
    return NULL;
}

void  task_internals::delete_thread(void* arg) 
{
    delete (task*)arg;
}

task* task::create(fptr f, void* arg, priority pri, size_t stack_size)
{ 
    task* t = new task;
    pthread_attr_init(&t->thread_attr);
#if !defined(__linux__)
    SYSCHECK(pthread_attr_setstacksize(&t->thread_attr, 
				       stack_size <= PTHREAD_STACK_MIN 
				       ? PTHREAD_STACK_MIN : stack_size));
#endif
#ifdef PTHREAD_CREATE_DETACHED
    SYSCHECK(pthread_attr_setdetachstate(&t->thread_attr, PTHREAD_CREATE_DETACHED));
#endif
#if !defined(__FreeBSD__)
    struct sched_param sp;
    sp.sched_priority = PRI_OTHER_MIN + 
	(PRI_OTHER_MAX - PRI_OTHER_MIN) * (pri - pri_background) 
	/ (pri_realtime - pri_background);  
    SYSCHECK(pthread_attr_setschedparam(&t->thread_attr, &sp)); 
#endif

    t->f = f;
    t->arg = arg; 

    SYSCHECK(pthread_create(&t->thread, &t->thread_attr, create_thread, t)); 
    return t;
}
 
void task::initialize(size_t) 
{
    static task main;
    SYSCHECK(pthread_key_create(&thread_key, delete_thread));
    SYSCHECK(pthread_setspecific(thread_key, &main));
#ifndef PRI_OTHER_MAX
    PRI_OTHER_MIN = sched_get_priority_min(SCHED_OTHER);
    PRI_OTHER_MAX = sched_get_priority_max(SCHED_OTHER);
#endif
}

void task::reschedule() {}

void task::reschedule(priority) {}

task* task::current() 
{ 
    return (task*)pthread_getspecific(thread_key);
}

void task::exit() 
{
    pthread_exit(NULL);
} 

void task::sleep(timeout_t msec)
{
    static event never_happened;
    never_happened.wait_with_timeout(msec);
}
