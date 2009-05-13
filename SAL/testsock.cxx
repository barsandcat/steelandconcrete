//-< TESTSOCK.CXX >--------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:     24-Oct-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Program for testing sockets performance
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
    socket_t* sock;
    boolean server = False;
    int i;
    task::initialize(task::normal_stack);

    if (argc < 3)
    {
        console::output("Socket test program\n"
                        "Usage: testsock [(local-server|global-server)|client] host:port\n");
        return EXIT_FAILURE;
    }

    if (*argv[1] == 'l' || *argv[1] == 'L')   // local server
    {
        socket_t* gate = socket_t::create_local(argv[2]);
        if (!gate->is_ok())
        {
            gate->get_error_text(errbuf, sizeof errbuf);
            console::error("Failed to create local socket: %s\n", errbuf);
        }
        sock = gate->accept();
        if (sock == NULL)
        {
            gate->get_error_text(errbuf, sizeof errbuf);
            console::error("Failed to accept socket: %s\n", errbuf);
        }
        delete gate;
        server = True;
    }
    else if (*argv[1] == 'g' || *argv[1] == 'G')   // global server
    {
        socket_t* gate = socket_t::create_global(argv[2]);
        if (!gate->is_ok())
        {
            gate->get_error_text(errbuf, sizeof errbuf);
            delete gate;
            console::error("Failed to create global socket: %s\n", errbuf);
        }
        sock = gate->accept();
        if (sock == NULL)
        {
            gate->get_error_text(errbuf, sizeof errbuf);
            delete gate;
            console::error("Failed to accept socket: %s\n", errbuf);
        }
        delete gate;
        server = True;
    }
    else   // client
    {
        sock = socket_t::connect(argv[2]);
        if (sock == NULL)
        {
            console::error("Failed to connect to server\n");
        }
        else if (!sock->is_ok())
        {
            sock->get_error_text(errbuf, sizeof errbuf);
            console::error("Connection to server failed: %s\n", errbuf);
        }
        server = False;
    }
    time_t start_time = time(NULL);

    if (server)
    {
        i = 0;
        while (sock->read(buf, HDR_SIZE) &&
                sock->read(buf, sizeof(buf) - HDR_SIZE))
        {
            i += 1;
            if (!sock->write(buf, sizeof buf))
            {
                sock->get_error_text(errbuf, sizeof errbuf);
                console::error("Write to socket failed: %s\n", errbuf);
            }
        }
        console::output("Handle %d requests\n", i);
    }
    else
    {
        for (i = 0; i < N_LOOPS; i++)
        {
            if (!sock->write(buf, sizeof buf) ||
                    !sock->read(buf, HDR_SIZE) ||
                    !sock->read(buf, sizeof(buf) - HDR_SIZE))
            {
                sock->get_error_text(errbuf, sizeof errbuf);
                console::error("Write to socket failed: %s\n", errbuf);
            }
        }
    }
    console::output("Elapsed time %d seconds\n", time(NULL) - start_time);
    return EXIT_SUCCESS;
}


