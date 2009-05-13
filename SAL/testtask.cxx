//-< TESTTASK.CXX >--------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:     27-Oct-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Program for testing multitasking performance
//-------------------------------------------------------------------*--------*

#include "console.h"
#include "task.h"
#include <time.h>

int n_active_tasks = 4;
int n_tasks = 20;
int n_loops = 1000000;

struct process_data
{
    mutex     cs;
    semaphore sem;
};

process_data* process;
event termination_event;
mutex termination_cs;

void task_proc process_function(void* arg)
{
    static int terminated = 0;
    int id = (long)arg;
    int n = n_loops;
    while (--n >= 0)
    {
        process[id].sem.wait();
        int wakeup_id = (id + n_active_tasks) % n_tasks;
        process[wakeup_id].cs.enter();
        process[wakeup_id].sem.signal();
        process[wakeup_id].cs.leave();
    }
    termination_cs.enter();
    if (++terminated == n_tasks)
    {
        termination_event.signal();
    }
    termination_cs.leave();
}

int main(int argc, char* argv[])
{
    int i;
    if ((argc >= 2 && sscanf(argv[1], "%i", &n_loops) != 1) ||
            (argc >= 3 && sscanf(argv[2], "%i", &n_tasks) != 1) ||
            (argc >= 4 && sscanf(argv[3], "%i", &n_active_tasks) != 1))
    {
        console::output("Task mutlitasking\n"
                        "Usage: testtask [loops [tasks [activations]]]\n");
        return EXIT_FAILURE;
    }
    if (n_tasks < n_active_tasks)
    {
        console::output("Number of activations should be not greater"
                        " than number of tasks\n");
        return EXIT_FAILURE;
    }
    task::initialize(task::normal_stack);
    process = new process_data[n_tasks];
    time_t start_time = time(NULL);
    for (i = 0; i < n_tasks; i++)
    {
        task::create(process_function, (void*)i);
    }
    for (i = 0; i < n_active_tasks; i++)
    {
        process[i].sem.signal();
    }
    termination_event.wait();
    console::output("Elapsed time for %d iteractions, %d tasks "
                    "and %d concurrent activations: %ld seconds\n",
                    n_loops, n_tasks, n_active_tasks, time(NULL) - start_time);
    termination_cs.enter();
    delete[] process;
    termination_cs.leave();
    return EXIT_SUCCESS;
}







