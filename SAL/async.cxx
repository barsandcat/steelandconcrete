//-< ASYNC.CXX >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Asyncronous event manager for cooperative multitasking in Unix 
//-------------------------------------------------------------------*--------*

#include "async.h"

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

unsigned  async_event_manager::n_desc;
fd_set    async_event_manager::input_desc;
fd_set    async_event_manager::output_desc;
timeout_t async_event_manager::select_timeout;
l2elem    async_event_manager::active_timers; 

dnm_array<unix_socket*> async_event_manager::sockets;

void async_event_manager::attach_input_channel(unix_socket* s)
{
    if (unsigned(s->fd) >= n_desc) { 
	n_desc = s->fd + 1;
    }
    sockets[s->fd] = s; 
    FD_SET(s->fd, &input_desc);
}

void async_event_manager::detach_input_channel(unix_socket* s)
{
    if (unsigned(s->fd) < n_desc) { 
	FD_CLR(s->fd, &input_desc);
    }
}

void async_event_manager::attach_output_channel(unix_socket* s)
{
    if (unsigned(s->fd) >= n_desc) { 
	n_desc = s->fd + 1;
    }
    sockets[s->fd] = s; 
    FD_SET(s->fd, &output_desc);
}

void async_event_manager::detach_output_channel(unix_socket* s)
{
    if (unsigned(s->fd) < n_desc) { 
	FD_CLR(s->fd, &output_desc);
    }
}

void async_event_manager::add_timer(ctimer* tmr)
{
    tmr->link_after(&active_timers);
    if (tmr->timeout <= select_timeout-1) { 
	select_timeout = tmr->timeout;
    }
}


void async_event_manager::select(boolean wait) 
{
    int rc;
    int n = n_desc;
    static fd_set  readfds;
    static fd_set  writefds;
    struct timeval timeout;

    readfds = input_desc;
    writefds = output_desc;
    
    if (wait) { 
	timeout.tv_sec = select_timeout/1000;
	timeout.tv_usec = select_timeout%1000*1000;
    } else { 
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
    } 
    rc = ::select(n, &readfds, &writefds, NULL, 
		  !wait || select_timeout ? &timeout : (struct timeval*)0);
    
    unix_socket** spp = &sockets; 

    if (rc < 0) {
	if (errno == EBADF) { 
	    static struct timeval nowait;
	    FD_ZERO(&readfds); 
	    FD_ZERO(&writefds); 
	    /* Try to find bad descriptor */
	    for (int i = 0; i < n; i++) { 
		if (FD_ISSET(i, &input_desc)) {
		    FD_SET(i, &readfds);
		    while ((rc=::select(i+1, &readfds, NULL, NULL, &nowait))<0
			   && errno == EINTR);
		    if (rc != 0 && errno == EBADF) { 
			FD_CLR(i, &input_desc);
			spp[i]->input_sem.signal(); 			
		    }
		    FD_CLR(i, &readfds);
		}
		if (FD_ISSET(i, &output_desc)) {
		    FD_SET(i, &writefds);
		    while ((rc=::select(i+1, NULL, &writefds, NULL, &nowait))<0
			   && errno == EINTR);
		    if (rc != 0 && errno == EBADF) { 
			FD_CLR(i, &output_desc);
			spp[i]->output_sem.signal(); 			
		    }
		    FD_CLR(i, &writefds);
		}
	    }
	}    
	return;
    }

    while (--n >= 0) { 
	if (FD_ISSET(n, &readfds)) { 
	    spp[n]->input_sem.signal(); 
	}
	if (FD_ISSET(n, &writefds)) { 
	    spp[n]->output_sem.signal(); 
	}
    }

    if (select_timeout) { 
	struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
	timeout_t current_time = cur_tv.tv_sec*1000 + cur_tv.tv_usec/1000;

	ctimer *tmr = (ctimer*)active_timers.next;
	select_timeout = 0;
	while (tmr != &active_timers) { 
	    ctimer* next = (ctimer*)tmr->next;
	    timeout_t delta = tmr->start + tmr->timeout - current_time; 

	    if (int(delta) <= 0) { 
		tmr->expired();
	    } else { 
		if (delta <= select_timeout-1) {
		    select_timeout = delta; 
		}
	    }
	    tmr = next; 
	}
    }
}



