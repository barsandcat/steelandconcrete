//-< CTASK.H >-------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Cooperative multitasking impelmentation (portable version)
//-------------------------------------------------------------------*--------*

#ifndef __CTASK_H__
#define __CTASK_H__

#include "support.h"

#define COOPERATIVE_MULTITASKING 1

#include <setjmp.h>

class task;
class mutex;
class semaphore;
class mutex_internals;
class task_internals;

class task_queue { 
    friend class task;
    friend class task_internals;
  protected: 
    task_internals* queue;

  public:
    void    insert(task_internals* tp);  
    void    remove(task_internals* tp);

    task_internals* first();

    boolean is_empty() { return queue == NULL; }
    void    prune() { queue = NULL; }

    task_queue() { queue = NULL; }
}; 

class semaphore_internals { 
  protected: 
    friend class task;
    friend class task_internals;

    task_queue wait_queue; 
    int        count;

    void wait(); 
    boolean wait_with_timeout(timeout_t msec); 
    void signal(); 
    void broadcast(); 

    semaphore_internals(int init_count) { count = init_count; }
};

class event_internals : public semaphore_internals { 
  protected: 
    friend class task;
    friend class task_internals;

    void wait(); 
    boolean wait_with_timeout(timeout_t msec); 
    void signal();
    void reset() { count = 0; }

    event_internals(boolean signaled) : semaphore_internals(signaled) {}
};

class mutex_internals : public semaphore_internals {
  protected: 
    friend class semaphorex_internals;
    friend class eventex_internals;
    friend class task_internals;

    task_internals*  owner;     // owner of mutex (NULL if none)
    mutex_internals* next_hold; // list of mutexes owned by task
    int              nest;      // nested access counter

    void release(); // release mutex hold by terminated task
    void enter(); 
    void leave(); 


    mutex_internals() : semaphore_internals(0) { 
	owner = NULL; 
	nest = 0;
    }
};

class semaphorex_internals : public semaphore_internals { 
  protected: 
    mutex_internals& cs;
    
    void wait() { 
	if (count > 0) { 
	    count -= 1;
	} else { 
	    cs.nest -= 1;
	    assert(cs.nest == 0);
	    cs.release();
	    semaphore_internals::wait();
	    cs.enter();
	}
    }
    boolean wait_with_timeout(timeout_t msec) { 
	if (count > 0) { 
	    count -= 1;
	    return True;
	} else { 
	    cs.nest -= 1;
	    assert(cs.nest == 0);
	    cs.release();
	    boolean result = semaphore_internals::wait_with_timeout(msec);
	    cs.enter();
	    return result;
	}	
    }
    semaphorex_internals(mutex_internals& guard, int init_count) 
    : semaphore_internals(init_count), cs(guard) {}
};    
	
class eventex_internals : public event_internals { 
  protected: 
    mutex_internals& cs;
    
    void wait() { 
	cs.nest -= 1;
	assert(cs.nest == 0);
	cs.release();
	event_internals::wait();
	cs.enter();
    }
    boolean wait_with_timeout(timeout_t msec) { 
	cs.nest -= 1;
	assert(cs.nest == 0);
	cs.release();
	boolean result = event_internals::wait_with_timeout(msec);
	cs.enter();
	return result;
    }	
    eventex_internals(mutex_internals& guard, boolean signaled) 
    : event_internals(signaled), cs(guard) {}
};

//
// Timer for cooperative multitasking.
// Asynchronous event manager is responsible for checking
// timeout expiration and calling 'expired' method of timer. 
// 
typedef void (*ctimer_callback_t)(void*);

class ctimer : public l2elem { 
    friend class async_event_manager;
  protected: 
    timeout_t         start;    // time of timer starting
    timeout_t         timeout;  
    void*             arg;
    ctimer_callback_t f;
    
    void expired();

  public:
    boolean timeout_expired; 
    void start_timer(ctimer_callback_t f, void* arg, timeout_t timeout); 
    void stop_timer() { unlink(); }
    void reset() { prune(); }
}; 

#define task_proc // qualifier for task start procedure

//
// Functions to support task debuuging. To see state of the tasks
// set breakpoint to function debug_catch_task_activation and
// then execute in debugger function debug_select_task(i), where
// 'i' is task number. This function restores context of specified task and 
// then function debug_catch_task_activation is called. 
// Total number of tasks in process (active or sleeping)
// can be obtained by function debug_get_number_of_tasks(). 
// Task number 'i' should be positive number less than value returned by
// debug_get_number_of_tasks(). It is possible to call function
// debug_select_task(i) several times. But it is impossible 
// to restore state of current task (task which was running when debugger
// stop the program), because it's context was not saved. 
// So it is impossible to continue execution of program after
// debug_select_task(i) was called. 
//
extern "C" int  debug_get_number_of_tasks();
extern "C" int  debug_select_task(int i);
extern "C" void debug_catch_task_activation(); 


class task_internals { 
    friend class task;
    friend class task_queue;
    friend class semaphore_internals;
    friend class event_internals;
    friend class mutex_internals;

    friend int debug_get_number_of_tasks();
    friend int debug_select_task(int i);

  protected: 
    unsigned        lomem_guard;   // special field for checking stack overflow
    
    task_internals* next_block;    // next free block
    boolean         used;          // used or free frames
    size_t          size;          // size of frame

    task_internals* chain;         // next task in running or waiting queue
    int             pri;           // priority of task

    void (task_proc *f)(void*);    // pointer to task function 
    jmp_buf         ctx;           // saved context of task
    jmp_buf         exit_ctx;      // used to perform task termination
    size_t          stack_size;    // size of stack used by task <= size
    void*           arg;           // argumrnt to task function
     
    mutex_internals* hold_mutexes; // list of mutexes which are hold by task

    semaphore_internals* wait_sem; // semaphore for which task is waiting

    ctimer          tmr;           // timer for waiting semaphore with timeout 
    boolean         timeout_expired; // task is wakeuped from sleeping at 
                                     // semaphore due to timeout expiration

    enum task_state {
	tsk_run,                  // task is executed now
	tsk_ready,                // task is ready to run (in ready_queue)
	tsk_wait,                 // task waiting for resource
	tsk_zombie                // killed task
    };  
    task_state      state;         // current satte of task

    unsigned        himem_guard;   // special field for checking stack overflow 

    static task_queue ready_queue; // queue of ready to run tasks
    static task_internals* curr;   // running task
    static task_internals  main;   // main task (created by intialize function)

    static jmp_buf  creator_ctx;   // context of task creating new task

    static void task_proc select_loop(void*);

    void switch_context(); // save context of current task and activate
                           // and schedule task with the highest priority 
                           // for running

    void allocate(size_t requested_stack_size); 

    //
    // Wait until semaphore will be signaled
    //
    void wait(semaphore_internals* sem);
    //
    // Wait until semaphore will be signaled or timeout will be expired.
    // Returns True in first case and False in second (timeout expiration).
    //
    boolean wait(semaphore_internals* sem, timeout_t msec);

    void wakeup(); // wakeup sleeping task due to timeout expiration
    static void timeout_expired_callback(void* tmr);

    void signal(); // switch sleeping at semaphore task to ready state 
                   // because semaphore is signaled

    void kill();
};

#endif




