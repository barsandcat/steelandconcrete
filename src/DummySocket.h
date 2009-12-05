#ifndef DUMMYSOCKET_H
#define DUMMYSOCKET_H

#include <sockio.h>


class DummySocket : public socket_t
{
public:
    DummySocket();
    ~DummySocket();
    virtual int       read(void* buf, size_t min_size, size_t max_size,
                           time_t timeout = WAIT_FOREVER) {}
    virtual boolean   read(void* buf, size_t size) {}
    virtual boolean   write(void const* buf, size_t size) {}

    virtual boolean   sendto(int ip_addr, int ip_port, void const* buf, size_t size) {}
    virtual int       recvfrom(int& ip_addr, int& ip_port, void* buf, size_t size) {}

    virtual boolean   is_ok() {}
    virtual void      get_error_text(char* buf, size_t buf_size) {}

    //
    // This method is called by server to accept client connection
    //
    virtual socket_t* accept() {}

    //
    // Cancel accept operation and close socket
    //
    virtual boolean   cancel_accept() {}

    //
    // Shutdown socket: prohibite write and read operations on socket
    //
    virtual boolean   shutdown() {}

    //
    // Close socket
    //
    virtual boolean   close() {}
protected:
private:
};

#endif // DUMMYSOCKET_H
