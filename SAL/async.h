//-< ASYNC.H >-------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Asyncronous event manager for cooperative multitasking in Unix 
//-------------------------------------------------------------------*--------*

#ifndef __ASYNC_H__
#define __ASYNC_H__

#if defined(__CYGWIN__)
#include <sys/select.h>
#endif

#if defined(__FreeBSD__)
#include <sys/types.h>
#endif

#include "task.h"
#include "unisock.h"
#include "support.h"

class async_event_manager { 
  private:
    static unsigned n_desc;
    static fd_set input_desc; 
    static fd_set output_desc; 
    
    static timeout_t select_timeout; 

    static dnm_array<unix_socket*> sockets; // attached sockets
    static l2elem active_timers; 

  public: 
    static void add_timer(ctimer* tmr);

    static void attach_input_channel(unix_socket* s);
    static void detach_input_channel(unix_socket* s);
    static void attach_output_channel(unix_socket* s);
    static void detach_output_channel(unix_socket* s);

    static void select(boolean wait);
}; 

#endif





