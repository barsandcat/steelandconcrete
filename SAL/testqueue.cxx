//-< TESTQUEUE.CXX >-------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:     2-Feb-2000   K.A. Knizhnik  * / [] \ *
//                          Last update: 2-Feb-2000    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Program for testing interprocess queue
//-------------------------------------------------------------------*--------*

#include "console.h"
#include "task.h"
#include <assert.h>
#include <time.h>

struct buffer { 
    int code;
};

fifo_queue<buffer> queue(100);

const int n_messages = 1000000;
event ready;

void task_proc consumer(void*) { 
    buffer buf;
    for (int i = 0; i < n_messages; i++) { 
	queue >> buf;
	assert(buf.code == i);
    }
    ready.signal();
}

void task_proc producer(void*) { 
    buffer buf;
    for (int i = 0; i < n_messages; i++) { 
	buf.code = i;
	queue << buf;
    }
}

int main()
{ 
    task::initialize(task::normal_stack);
    time_t start = time(NULL);
    task::create(producer, NULL);
    task::create(consumer, NULL);
    ready.wait();
    console::output("Elapsed time for %d messages: %d seconds\n", 
		    n_messages, (int)(time(NULL)-start));
    return EXIT_SUCCESS;
}







