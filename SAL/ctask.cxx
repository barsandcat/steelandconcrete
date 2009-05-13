//-< CTASK.CXX >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Algorithm of portable multitasking is taken from Stig Kofoed article in
// DDJ of Novemeber 1995.
//-------------------------------------------------------------------*--------*

#include "task.h"
#include "async.h"
#include <sys/time.h>
#include <sys/types.h>

//
// Append protected page to task stack area.
//
//#define CHECK_STACK_OVERFLOW 1

#ifdef CHECK_STACK_OVERFLOW
#include <unistd.h>
#include <sys/mman.h>
static int page_size;
#endif

//
// Defining this name adds very little overhead to program but make
// it possible to use normal C debugger for debugging multitasking application.
//
//#define CTASK_DEBUGGING_SUPPORT 1

static int stack_guard;

#if defined(__osf__) || defined(__FreeBSD__)
#define longjmp(b,s) _longjmp(b,s) // do not restore signal context
#define setjmp(b)  _setjmp(b)
#endif

//
// Task prioritized queue
//

inline void task_queue::insert(task_internals* tp)
{
    task_internals **tpp;
    for (tpp = &queue;
            *tpp != NULL && (*tpp)->pri >= tp->pri;
            tpp = &(*tpp)->chain);
    tp->chain = *tpp;
    *tpp = tp;
}

inline void task_queue::remove(task_internals* tp)
{
    task_internals **tpp;
    for (tpp = &queue; *tpp != tp; tpp = &(*tpp)->chain);
    *tpp = tp->chain;
}

inline task_internals* task_queue::first()
{
    task_internals* t = queue;
    if (t != NULL)
    {
        queue = t->chain;
    }
    return t;
}

//
// Timer
//


void ctimer::expired()
{
    stop_timer();
    (*f)(arg);
}

void ctimer::start_timer(ctimer_callback_t cb, void* cb_arg, timeout_t msec)
{
    struct timeval cur_tv;
    gettimeofday(&cur_tv, NULL);
    start = cur_tv.tv_sec * 1000 + cur_tv.tv_usec / 1000;
    timeout = msec;
    f = cb;
    arg = cb_arg;
    async_event_manager::add_timer(this);
}

//
// Task
//

task_queue      task_internals::ready_queue;
task_internals* task_internals::curr;
task_internals  task_internals::main;
jmp_buf         task_internals::creator_ctx;


#define    GUARD_VALUE   0xDEADDEED

void task_internals::switch_context()
{
    assert(curr->himem_guard == GUARD_VALUE &&
           curr->lomem_guard == GUARD_VALUE);

    if (setjmp(ctx) == 0)
    {
        curr = ready_queue.first();
        assert(curr != NULL /* deadlock*/);
        curr->state = tsk_run;

        // Check stack overflow
        assert(curr->himem_guard == GUARD_VALUE &&
               curr->lomem_guard == GUARD_VALUE);

        assert(curr->used);

        longjmp(curr->ctx, 1);
    }
#ifdef CTASK_DEBUGGING_SUPPORT
    else debug_catch_task_activation();
#endif
}

inline size_t distance(task_internals* from, task_internals* to)
{
    return (char*)from >= (char*)to
           ? (char*)from - (char*)to    // stack grows up
           : (char*)to - (char*)from;   // stack grows down
}


void task_internals::allocate(size_t requested_stack_size)
{
    task   t;
    size_t d = distance(this, &t);

    if (d < requested_stack_size)
    {
        allocate(requested_stack_size); // never returns
    }

    t.lomem_guard = t.himem_guard = GUARD_VALUE;
    t.size = size - d;
    size = d;
    t.used = False;
    t.next_block = next_block;
    next_block = &t;

    if (setjmp(t.ctx) == 0)   // wait stack allocation request (from create)
    {
        longjmp(ctx, 1);      // return to requester
    }
    while (True)
    {
        if (t.stack_size + task::min_stack <= t.size)
        {
            // split frame
            if (setjmp(t.ctx) == 0)   // previouse 'longjmp' will jump here
            {
                t.allocate(t.stack_size);
            }
        }
        t.used = True;

        if (setjmp(t.ctx) == 0)   // wait untill task be activated by scheduler
        {
            longjmp(creator_ctx, 1); // return control to creator
        }

#ifdef CHECK_STACK_OVERFLOW
        long stack_base;
        if ((char*)this > (char*)&t)   // stack grows down
        {
            stack_base = ((long)(&t + 1) - t.stack_size + page_size - 1)
                         & ~(page_size - 1);
        }
        else   // stack grows up
        {
            stack_base = ((long) & t + t.stack_size - page_size)
                         & ~(page_size - 1);
        }
        mprotect((void*)stack_base, page_size, PROT_NONE);
#endif
        if (setjmp(t.exit_ctx) == 0)
        {
            (*t.f)(t.arg);
            while (t.hold_mutexes != NULL)
            {
                t.hold_mutexes->release();
            }
        }
#ifdef CHECK_STACK_OVERFLOW
        mprotect((void*)stack_base, page_size, PROT_READ | PROT_WRITE);
#endif

        t.used = False;
        if (t.next_block != NULL && !t.next_block->used)
        {
            t.size += t.next_block->size; // merge with the following frame
            t.next_block = t.next_block->next_block;
        }
        // locate previouse frame
        task_internals* tp;
        for (tp = &main; tp->next_block != &t; tp = tp->next_block);
        if (tp != &main && !tp->used)
        {
            tp->size += t.size;
            tp->next_block = t.next_block;
        }

        t.switch_context();
    }
}

void task_proc task_internals::select_loop(void*)
{
    while (True)
    {
        async_event_manager::select(ready_queue.is_empty());
        task::reschedule();
    }
}

void task::initialize(size_t main_stack_size)
{
    task_internals t;
#ifdef CHECK_STACK_OVERFLOW
    page_size = getpagesize();
    stack_guard = page_size * 2;
    main_stack_size += stack_guard;
#endif

    t.size = LONG_MAX; // total stack area is not limited
    t.next_block = NULL;
    t.lomem_guard = t.himem_guard = GUARD_VALUE;
    if (setjmp(t.ctx) == 0)
    {
        t.allocate(main_stack_size);
    }
#ifdef CHECK_STACK_OVERFLOW
    long stack_base;
    if ((char*)t.next_block < (char*)&t)   // stack grows down
    {
        stack_base = ((long)(t.next_block + 1) + page_size - 1) & ~(page_size - 1);
    }
    else
    {
        stack_base = ((long)t.next_block - page_size) & ~(page_size - 1);
    }
    mprotect((void*)stack_base, page_size, PROT_NONE);
#endif

    main = t;
    main.used = True;
    main.pri = pri_normal;
    main.state = tsk_run;
    main.wait_sem = NULL;
    main.hold_mutexes = NULL;
    curr = &main;
    create(select_loop, NULL, pri_background);
}

task* task::create(fptr f, void* arg, priority pri, size_t stack_size)
{
    stack_size += stack_guard;
    assert(stack_size >= size_t(min_stack));

    for (task_internals* tp = main.next_block; tp != NULL; tp = tp->next_block)
    {
        if (!tp->used && tp->size >= stack_size)
        {
            tp->f = f;
            tp->arg = arg;
            tp->stack_size = stack_size;
            tp->pri = pri;
            tp->wait_sem = NULL;
            tp->hold_mutexes = NULL;

            if (setjmp(creator_ctx) == 0)
            {
                longjmp(tp->ctx, 1);
            }
            tp->state = tsk_ready;
            ready_queue.insert(tp);
            return (task*)tp;
        }
    }
    return NULL;
}

void task::reschedule()
{
    if (!ready_queue.is_empty() && curr->pri <= ready_queue.queue->pri)
    {
        curr->state = tsk_ready;
        ready_queue.insert(curr);
        curr->switch_context();
    }
}

void task::reschedule(priority pri)
{
    if (!ready_queue.is_empty() && pri <= ready_queue.queue->pri)
    {
        curr->state = tsk_ready;
        priority save_pri = (priority)curr->pri;
        curr->pri = pri;
        ready_queue.insert(curr);
        curr->pri = save_pri;
        curr->switch_context();
    }
}

task* task::current()
{
    return (task*)task_internals::curr;
}

void task_internals::kill()
{
    if (this == &main)
    {
        ::exit(EXIT_SUCCESS);
    }
    if (state == tsk_wait)
    {
        tmr.stop_timer();
        wait_sem->wait_queue.remove(this);
        wait_sem = NULL;
    }
    while (hold_mutexes != NULL)
    {
        hold_mutexes->release();
    }
    if (state == tsk_run)
    {
        state = tsk_zombie;
        longjmp(exit_ctx, 1);
    }
    memcpy(ctx, exit_ctx, sizeof ctx);
    state = tsk_zombie;
}

void  task::exit()
{
    task_internals::curr->kill();
}

void  task::sleep(timeout_t msec)
{
    static event never_happened;
    never_happened.wait_with_timeout(msec);
}

inline void task_internals::wakeup()
{
    assert(state == tsk_wait);
    wait_sem->wait_queue.remove(this);
    state = tsk_ready;
    ready_queue.insert(this);
    timeout_expired = True;
}

inline boolean task_internals::wait(semaphore_internals* sem, timeout_t msec)
{
    tmr.start_timer(timeout_expired_callback, this, msec);
    wait_sem = sem;
    sem->wait_queue.insert(this);
    timeout_expired = False;
    state = tsk_wait;
    switch_context();
    return !timeout_expired;
}

inline void task_internals::wait(semaphore_internals* sem)
{
    tmr.reset();
    wait_sem = sem;
    sem->wait_queue.insert(this);
    curr->state = tsk_wait;
    switch_context();
}

inline void task_internals::signal()
{
    tmr.stop_timer();
    wait_sem = NULL;
    state = tsk_ready;
    ready_queue.insert(this);
}

void task_internals::timeout_expired_callback(void* arg)
{
    ((task_internals*)arg)->wakeup();
}

//
// Semaphore implementaion
//

void semaphore_internals::wait()
{
    if (count > 0)
    {
        count -= 1;
    }
    else
    {
        task_internals::curr->wait(this);
    }
}

boolean semaphore_internals::wait_with_timeout(timeout_t msec)
{
    if (count > 0)
    {
        count -= 1;
        return True;
    }
    if (msec == 0)
    {
        return False;
    }
    else
    {
        return task_internals::curr->wait(this, msec);
    }
}

void semaphore_internals::signal()
{
    task_internals* t = wait_queue.first();
    if (t != NULL)
    {
        t->signal();
    }
    else
    {
        count += 1;
    }
}

//
// Events
//

void event_internals::signal()
{
    while (!wait_queue.is_empty())
    {
        wait_queue.first()->signal();
    }
    count = 1;
}

void event_internals::wait()
{
    if (count == 0)
    {
        task_internals::curr->wait(this);
    }
}

boolean event_internals::wait_with_timeout(timeout_t msec)
{
    if (msec == 0)
    {
        return count != 0;
    }
    else
    {
        if (count != 0)
        {
            return True;
        }
        else
        {
            return task_internals::curr->wait(this, msec);
        }
    }
}

//
// Mutex
//

void mutex_internals::enter()
{
    if (task_internals::curr != owner)
    {
        while (owner != NULL)
        {
            wait();
        }
        owner = task_internals::curr;
        next_hold = owner->hold_mutexes;
        owner->hold_mutexes = this;
    }
    nest += 1;
}

void mutex_internals::release()
{
    mutex_internals** mpp;

    for (mpp = &owner->hold_mutexes; *mpp != this; mpp = &(*mpp)->next_hold);
    *mpp = next_hold;
    owner = NULL;
    nest = 0;
    if (!wait_queue.is_empty())
    {
        signal();
    }
}

void mutex_internals::leave()
{
    assert(owner == task_internals::curr);
    if (--nest == 0)
    {
        release();
        task::reschedule();
    }
}



#ifdef CTASK_DEBUGGING_SUPPORT

//
// Set breakpoint to this function and call debug_select_task(int) function to
// see task backtrace (call stack).
//
void debug_catch_task_activation() {}

int debug_get_number_of_tasks()
{
    int n = 0;
    for (task_internals* tp = &task_internals::main;
            tp != NULL;
            tp = tp->next_block)
    {
        if (tp->used)
        {
            n += 1;
        }
    }
    return n;
}

//
// Switch context to specified task. Total number of active tasks in process
// can be obtained by debug_get_number_of_tasks() function. Index of task
// passed as parameter to this function should be positive number less than
// value returned by debug_get_number_of_tasks(). Task with index 0 refers
// to currently active task. This task can't be activated with this function.
// After specified task is activated, control is passed to function
// debug_catch_task_activation() and then execution of task is continued.
// If task index doesn't belong to valid range -1 is returned.
// Otherwise control from this function will not returned (longjmp).
//
int debug_select_task(int i)
{
    for (task_internals* tp = &task_internals::main;
            tp != NULL;
            tp = tp->next_block)
    {
        if (tp->used && tp->state != task_internals::tsk_run && --i == 0)
        {
            task_internals::curr = tp;
            longjmp(tp->ctx, 2);
        }
    }
    return -1;
}

#endif


