#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <ClientConnection.h>
#include <list>
#include <sockio.h>

class ServerGame;

class ConnectionManager
{
public:
    ConnectionManager(socket_t& aGate, ServerGame& aGame);
    bool IsAllClientsReady();
    ~ConnectionManager();
protected:
private:
    friend void ManagerThreadFunction(void* param);
    void NewConnection(socket_t& aSocket);
    bool mQuit;
    socket_t& mGate;
    ServerGame& mGame;
    std::list< ClientConnection* > mClients;
};

#endif // CONNECTIONMANAGER_H
