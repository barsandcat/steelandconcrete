//-< TASK.H >--------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 23-Apr-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Mutitasking and syncronization mechanisms abstractions
//-------------------------------------------------------------------*--------*

#ifndef __TASK_H__
#define __TASK_H__

#include "stdtp.h"

typedef unsigned timeout_t; // timeout in milliseconds

#if defined(_WIN32) && !defined(__CYGWIN__)
#include "wtask.h"
#elif defined(PTHREADS)
#include "ptask.h"
#else
#include "ctask.h"
#endif

//
// Mutual exclusion: only one task can hold mutex at the same time.
// Task holding mutex can call 'enter' any number of times and
// will hold mutex until correspondent number of 'leave' will be
// executed. If task holding mutex is terminated mutex is automaticaly unlocked
//
class mutex : private mutex_internals {
    friend class eventex;
    friend class semaphorex;
  public:
    void enter() { mutex_internals::enter(); }
    void leave() { mutex_internals::leave(); }
};

//
// Critical section. When object of this class is used as local
// variable, mutex is locked by constructor at the beginning of the block
// (entering critical section) and unlocked by destructor after existing from
// the block (leaving critical section)
//
class critical_section {
    mutex& cs;
  public:
    critical_section(mutex& guard) : cs(guard) {
	cs.enter();
    }
    ~critical_section() { cs.leave(); }
};




//
// Standard sempaphore supporting wait (P) and signal (V) operations.
//
class semaphore : private semaphore_internals {
  public:
    //
    // Wait semaphore to be signaled
    //
    void wait() { semaphore_internals::wait(); }
    //
    // Wait for signal for a specified period of time.
    // If timeout expired before semaphore is signaled then 'False'
    // is returned.
    //
    boolean wait_with_timeout(timeout_t msec) {
	return semaphore_internals::wait_with_timeout(msec);
    }
    //
    // Wakeup one task waiting for semaphore. If no task is waiting
    // then value of semaphore is increased so that following wait will
    // not block the task.
    //
    void signal() { semaphore_internals::signal(); }

    semaphore(int init_count = 0) : semaphore_internals(init_count) {}
};

//
// Semaphore with associated mutex
//
class semaphorex : private semaphorex_internals {
  public:
    //
    // Wait semaphore to be signaled. Should be called with associated mutex
    // locked. Before wait mutex is unlocked and will locked again before
    // returning.
    //
    void wait() { semaphorex_internals::wait(); }
    //
    // Wait for signal for a specified period of time.
    // If timeout expired before semaphore is signaled then 'False'
    // is returned. Should be called with associated mutex
    // locked. Before wait mutex is unlocked and will locked again before
    // returning.
    //
    boolean wait_with_timeout(timeout_t msec) {
	return semaphorex_internals::wait_with_timeout(msec);
    }
    //
    // Wakeup one task waiting for semaphore. If no task is waiting
    // then value of semaphore is increased so that following wait will
    // not block the task. Should be called with associated mutex locked.
    //
    void signal() { semaphorex_internals::signal(); }

    semaphorex(mutex& cs, int init_count = 0)
    : semaphorex_internals(cs, init_count) {}
};

//
// Event with manual reset
//
class event : private event_internals {
  public:
    //
    // Wait event to be signaled
    //
    void wait() { event_internals::wait(); }
    //
    // Wait for signal for a specified period of time.
    // If timeout expired before event is signaled then 'False' is returned.
    //
    boolean wait_with_timeout(timeout_t msec) {
	return event_internals::wait_with_timeout(msec);
    }
    //
    // Switch event object to signaled state
    //
    void signal() { event_internals::signal(); }
    //
    // Reset event to non-signaled state.
    //
    void reset() { event_internals::reset(); }

    event(boolean signaled = False) : event_internals(signaled) {}
};

//
// Event with associated mutex
//
class eventex : private eventex_internals {
  public:
    //
    // Wait event to be signaled. Associated mutex is released before wait
    // and will be relocked before returning.
    //
    void wait() { eventex_internals::wait(); }
    //
    // Wait for signal for a specified period of time. Associated mutex
    // is released before wait and will be relocked before returning.
    // If timeout expired before event is signaled then 'False' is returned.
    //
    boolean wait_with_timeout(timeout_t msec) {
	return eventex_internals::wait_with_timeout(msec);
    }
    //
    // Switch event object to signaled state. Should be called with associated
    // mutex locked.
    //
    void signal() { eventex_internals::signal(); }
    //
    // Reset event to non-signaled state. Should be called with associated
    // mutex locked.
    //
    void reset() { eventex_internals::reset(); }

    eventex(mutex& cs, boolean signaled = False)
    : eventex_internals(cs, signaled) {}
};

//
// Task is a unit for scheduling excution. Schedulig can be either preemptive
// or cooperative. After task termination all mutexes hold by this task
// will be released
//
class task : private task_internals {
    friend class task_internals;
  public:
    typedef void (task_proc *fptr)(void* arg);
    enum priority {
	pri_background,
	pri_low,
	pri_normal,
	pri_high,
	pri_realtime
    };
    enum {
	min_stack    = 8*1024,
	small_stack  = 16*1024,
	normal_stack = 64*1024,
	big_stack    = 256*1024,
	huge_stack   = 1024*1024
    };
    //
    // Create new task. Pointer to task object returned by this function
    // can be used only for thread identification.
    //
    static task* create(fptr f, void* arg = NULL, priority pri = pri_normal,
	 size_t stack_size = normal_stack);
    //
    // Initialize tasking.
    //
    static void  initialize(size_t main_stack_size = normal_stack);
    //
    // Force task rescheduling
    //
    static void  reschedule();
    //
    // Force rescheduling with temporary asignemtn of specified priority
    // to the current task
    //
    static void  reschedule(priority pri);
    //
    // Terminate current task
    //
    static void  exit();
    //
    // Current task will sleep during specified period
    //
    static void  sleep(timeout_t msec);
    //
    // Get current task
    //
    static task* current();
};


template<class T>
class fifo_queue {
  protected:
    struct queue_end {
	semaphorex       sem;
	volatile size_t  pos;
	volatile int     wait;

	queue_end(mutex& cs) : sem(cs) { pos = 0; wait = 0; }
    };
    mutex        cs;
    queue_end    put, get;
    T*           buf;
    const size_t buf_size;

  public:
    boolean is_empty() const { return put.pos == get.pos; }
    boolean is_full() const {
	return put.pos == get.pos-1
	    || (get.pos == 0 && put.pos == buf_size-1);
    }
    fifo_queue& operator << (T const& elem) {
	cs.enter();
	while (is_full()) {
	    put.wait += 1;
	    put.sem.wait();
	}
	buf[put.pos] = elem;
	if (++put.pos == buf_size) put.pos = 0;
	if (get.wait != 0) {
	    get.sem.signal();
	    get.wait -= 1;
	}
	cs.leave();
	return *this;
    }
    fifo_queue& operator >> (T& elem) {
	cs.enter();
	while (is_empty()) {
	    get.wait += 1;
	    get.sem.wait();
	}
	elem = buf[get.pos];
	if (++get.pos == buf_size) get.pos = 0;
	if (put.wait) {
	    put.sem.signal();
	    put.wait -= 1;
	}
	cs.leave();
	return *this;
    }
    fifo_queue(size_t size) : put(cs), get(cs), buf_size(size) {
	buf = new T[size];
    }
    ~fifo_queue() { delete[] buf; }
};


class barrier {
  protected:
    mutex   cs;
    eventex e;
    int     count;

  public:
    void reset(int n) {
	count = n;
	e.reset();
    }
    void reach() {
	cs.enter();
	assert(count > 0);
	if (--count != 0) {
	    e.wait();
	} else {
	    e.signal();
	}
	cs.leave();
    }
    barrier() : e(cs) { count = 0; }
};

#endif
