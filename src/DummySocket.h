#ifndef DUMMYSOCKET_H
#define DUMMYSOCKET_H

#include <sockio.h>


class DummySocket : public socket_t
{
public:
    DummySocket();
    ~DummySocket();
    virtual int       read(void* buf, size_t min_size, size_t max_size,
                           time_t timeout = WAIT_FOREVER) { return 0; }
    virtual boolean   read(void* buf, size_t size) { return true; }
    virtual boolean   write(void const* buf, size_t size) { return true; }

    virtual boolean   sendto(int ip_addr, int ip_port, void const* buf, size_t size) { return true; }
    virtual int       recvfrom(int& ip_addr, int& ip_port, void* buf, size_t size) { return 0; }

    virtual boolean   is_ok() { return true; }
    virtual void      get_error_text(char* buf, size_t buf_size) {}

    //
    // This method is called by server to accept client connection
    //
    virtual socket_t* accept() { return 0; }

    //
    // Cancel accept operation and close socket
    //
    virtual boolean   cancel_accept() { return true; }

    //
    // Shutdown socket: prohibite write and read operations on socket
    //
    virtual boolean   shutdown() { return true; }

    //
    // Close socket
    //
    virtual boolean   close() { return true; }
protected:
private:
};

#endif // DUMMYSOCKET_H
