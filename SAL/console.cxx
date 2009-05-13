//-< CONSOLE.CXX >---------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Handle information and error messages at virtual console.
//-------------------------------------------------------------------*--------*

#include "console.h"

console* console::active_console;
static console default_console; 

console::console ()
{
    if (active_console == NULL || active_console == &default_console) { 
	active_console = this; 
    }
}

void console::output(const char* msg, ...) 
{
    va_list args;
    va_start (args, msg);
    assert(active_console != NULL);
    active_console->output_data(false, msg, args);
    va_end (args);
}

void console::error(const char* msg, ...) 
{
    va_list args;
    va_start (args, msg);
    assert(active_console != NULL);
    active_console->output_data(true, msg, args);
    va_end (args);
    abort();
}

boolean console::input(char* buf, size_t buf_size)
{
    return active_console->input_data(buf, buf_size);
}

void console::output_data(boolean, const char* msg, va_list args)
{
    vfprintf(stdout, msg, args);
}

#ifdef COOPERATIVE_MULTITASKING
#include <unistd.h>
#include "unisock.h"

boolean console::input_data(char* dst, size_t dst_size)
{
    static unix_socket stdin_socket(0);
    static char  buf[4096];
    static char* bp = buf;
    static int   used;

    if (dst_size == 0) { 
	return False;
    }
    int   src_size = used;
    char* src = bp;

    fflush(stdout);

    while (True) { 
	while (src_size > 0 && dst_size > 1) {
	    src_size -= 1;
	    dst_size -= 1;
	    if ((*dst++ = *src++) == '\n') { 
		*dst = '\0';
		bp = src; 
		used = src_size;
		return True;
	    }
	}
	if (dst_size == 1) { 
	    *dst++ = '\0';
	    bp = src;	
	    used = src_size;
	    return True;
	} else { 
	    stdin_socket.wait_input();
	    int rc = read(0, buf, sizeof buf);
	    if (rc > 0) { 
		src_size = rc; 
		src = buf;
	    } else { 
		used = 0;
		bp = buf;
		return False;
	    }
	}
    }
}

#else

boolean console::input_data(char* buf, size_t buf_size)
{
    return fgets(buf, buf_size, stdin) != NULL; 
} 

#endif
