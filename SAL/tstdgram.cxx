//-< TESTDGRAM.CXX >--------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:     24-Oct-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Program for testing datagram sockets performance
//-------------------------------------------------------------------*--------*

#include "console.h"
#include "sockio.h"
#include "task.h"
#include <time.h>

#define HDR_SIZE  16
#define BODY_SIZE 240
#define N_LOOPS   1000000

int main(int argc, char* argv[]) 
{
    char errbuf[256];
    char buf[HDR_SIZE+BODY_SIZE];	
    task::initialize(task::normal_stack);
    int ip_addr;
    int ip_port;
    int i;
    if (argc != 3) { 
     	console::output("Datagram socket test program\n"
			"Usage: tstdgram (send|recv) host:port\n");
	return EXIT_FAILURE;
    }
    socket_t* sock;
    time_t start_time = time(NULL);
    if (argv[1][0] == 's' || argv[1][0] == 'S') { 
	sock = socket_t::create_dgram();
	char* p = strchr(argv[2], ':');
	if (p == NULL) { 
	    console::output("Address should be 'host:port'\n");
	    return EXIT_FAILURE;
	}
	*p = '\0';
	ip_addr = sock->get_ip(argv[2]);
	if (ip_addr == -1 || sscanf(p+1, "%d", &ip_port) != 1) { 
	    console::output("Address should be 'host:port'\n");
	    return EXIT_FAILURE;
	}
	for (i = 0; i < N_LOOPS; i++) { 	
	    if (!sock->sendto(ip_addr, ip_port, buf, sizeof buf)) {
		sock->get_error_text(errbuf, sizeof errbuf);
		console::error("sendto failed: %s\n", errbuf);
	    }
	    if (sock->recvfrom(ip_addr, ip_port, buf, sizeof buf) != (int)(sizeof buf)) { 
		sock->get_error_text(errbuf, sizeof errbuf);
		console::error("recvfrom failed: %s\n", errbuf);
	    }
	}
    } else { 
	sock = socket_t::create_dgram(argv[2]);
	if (!sock->is_ok()) { 
	    sock->get_error_text(errbuf, sizeof errbuf);
	    console::error("sendto failed: %s\n", errbuf);
	    return EXIT_FAILURE;
	}
	for (i = 0; i < N_LOOPS; i++) { 	
	    if (sock->recvfrom(ip_addr, ip_port, buf, sizeof buf) != (int)(sizeof buf)) { 
		sock->get_error_text(errbuf, sizeof errbuf);
		console::error("recvfrom failed: %s\n", errbuf);
	    }
	    if (!sock->sendto(ip_addr, ip_port, buf, sizeof buf)) {
		sock->get_error_text(errbuf, sizeof errbuf);
		console::error("sendto failed: %s\n", errbuf);
	    }
	}
    }	
    console::output("Elapsed time %d seconds\n", time(NULL) - start_time);
    return EXIT_SUCCESS;
}


