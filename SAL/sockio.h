//-< SOCKIO.H >------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Socket abstraction
//-------------------------------------------------------------------*--------*

#ifndef __SOCKIO_H__
#define __SOCKIO_H__

#include "stdtp.h"
#include <time.h>

#define DEFAULT_CONNECT_MAX_ATTEMPTS 100
#define DEFAULT_RECONNECT_TIMEOUT    1  // seconds
#define DEFAULT_LISTEN_QUEUE_SIZE    5
#define LINGER_TIME                  10 // seconds
#define WAIT_FOREVER                 ((time_t)-1)
#define ENABLE_BROADCAST             1

//
// Abstract socket interface
//
class socket_t { 
  public: 
    virtual int       read(void* buf, size_t min_size, size_t max_size,
			   time_t timeout = WAIT_FOREVER) = 0;
    virtual boolean   read(void* buf, size_t size) = 0;
    virtual boolean   write(void const* buf, size_t size) = 0;

    virtual boolean   sendto(int ip_addr, int ip_port, void const* buf, size_t size) = 0;
    virtual int       recvfrom(int& ip_addr, int& ip_port, void* buf, size_t size) = 0;

    virtual boolean   is_ok() = 0; 
    virtual void      get_error_text(char* buf, size_t buf_size) = 0;

    //
    // This method is called by server to accept client connection
    //
    virtual socket_t* accept() = 0;

    //
    // Cancel accept operation and close socket
    //
    virtual boolean   cancel_accept() = 0;

    //
    // Shutdown socket: prohibite write and read operations on socket
    //
    virtual boolean   shutdown() = 0;

    //
    // Close socket
    //
    virtual boolean   close() = 0;

    //
    // Create client socket connected to local or global server socket
    //
    enum socket_domain { 
	sock_any_domain,   // domain is chosen automatically
	sock_local_domain, // local domain (i.e. Unix domain socket) 
	sock_global_domain // global domain (i.e. INET sockets) 
    };

    static socket_t*  connect(char const* address, 
			      socket_domain domain = sock_any_domain, 
			      int max_attempts = DEFAULT_CONNECT_MAX_ATTEMPTS,
			      time_t timeout = DEFAULT_RECONNECT_TIMEOUT);
    
    //
    // Create local domain TCP/IP socket
    //
    static socket_t*  create_local(char const* address,
				   int listen_queue_size = 
				       DEFAULT_LISTEN_QUEUE_SIZE);

    //
    // Create global domain TCP/IP socket 
    //
    static socket_t*  create_global(char const* address,
				    int listen_queue_size = DEFAULT_LISTEN_QUEUE_SIZE);
    //
    // Create UDP socket. 
    // If address is not NULL, ther socket will be bound to specified address.
    // Otherwise socket will be in unbound state.
    // To enable sending of broadcasts set ENABLE_BROADCAST flag.
    //
    static socket_t*  create_dgram(char const* address = NULL, int flags = 0);

    //
    // Get IP address of the host. "address" parameter should contain either
    // symbolic host name (for example "crabby"), either string with IP address
    // (for example "195.239.208.225")
    //
    static int get_ip(char const* address);

    virtual ~socket_t() {} 
    socket_t() { state = ss_close; }

  protected:
    enum { ss_open, ss_shutdown, ss_close } state;
};

// 
// Return current host name + identifier of current process
//
extern char const* get_process_name(); 

#endif



