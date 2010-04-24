//-< PTASK.H >-------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      2-Mar-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 26-Sep-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Tasking implemented using Posix pthreads
//-------------------------------------------------------------------*--------*

#ifndef __PTASK_H__
#define __PTASK_H__

#if defined(__FreeBSD__)
#include <sched.h>
#endif
#include <pthread.h>

#if defined(__osf__)
#define PTHREAD_GET_EXPIRATION_NP 1
// I had problems with recursive mutexes at our version of Digital Uni
#undef PTHREAD_MUTEX_RECURSIVE_NP
#endif

class mutex_internals {
    friend class eventex_internals;
    friend class semaphorex_internals;

  protected:
    pthread_mutex_t cs;
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
    void enter() { pthread_mutex_lock(&cs); }
    void leave() { pthread_mutex_unlock(&cs); }
#else
    int       count;
    pthread_t owner;
    void enter()
    {
	pthread_t self = pthread_self();
	if (owner != self) {
	    pthread_mutex_lock(&cs);
	    owner = self;
	}
	count += 1;
    }
    void leave()
    {
	assert(pthread_self() == owner);
	if (--count == 0) {
	    owner = 0;
	    pthread_mutex_unlock(&cs);
	}
    }
#endif

    mutex_internals();
    ~mutex_internals();
};

class semaphore_internals {
  protected:
    pthread_mutex_t cs;
    pthread_cond_t  cond;
    int             count;

    void wait();
    void signal();
    boolean wait_with_timeout(timeout_t msec);

    semaphore_internals(int init_count);
    ~semaphore_internals();
};

class semaphorex_internals {
  protected:
    mutex_internals& guard;
    pthread_cond_t   cond;
    int              count;

    void wait();
    void signal();
    boolean wait_with_timeout(timeout_t msec);

    semaphorex_internals(mutex_internals& cs, int init_count);
    ~semaphorex_internals();
};

class event_internals {
  protected:
    pthread_mutex_t cs;
    pthread_cond_t  cond;
    int             signaled;
    long            n_signals;

    void wait();
    boolean wait_with_timeout(timeout_t msec);
    void signal();
    void reset() { signaled = False; }

    event_internals(boolean signaled);
    ~event_internals();
};

class eventex_internals {
  protected:
    mutex_internals& guard;
    pthread_cond_t   cond;
    int              signaled;
    long             n_signals;

    void wait();
    boolean wait_with_timeout(timeout_t msec);
    void signal();
    void reset() { signaled = False; }

    eventex_internals(mutex_internals& cs, boolean signaled);
    ~eventex_internals();
};

#define task_proc // qualifier for thread procedure

class task_internals {
  protected:
    static pthread_key_t thread_key;

    pthread_t      thread;
    pthread_attr_t thread_attr;

    void*          arg;
    void           (*f)(void* arg);

    static void* create_thread(void* arg);
    static void  delete_thread(void* arg);
};

#endif
