#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <ClientConnection.h>
#include <list>
#include <sockio.h>
#include <boost/thread.hpp>

class ServerGame;

class ConnectionManager
{
public:
    ConnectionManager(socket_t& aGate, ServerGame& aGame);
    void operator()();
protected:
private:
    bool mQuit;
    socket_t& mGate;
    ServerGame& mGame;
};

#endif // CONNECTIONMANAGER_H
