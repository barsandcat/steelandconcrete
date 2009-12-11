#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <ClientConnection.h>
#include <list>
#include <sockio.h>
#include <task.h>

class ServerGame;

class ConnectionManager
{
public:
    ConnectionManager(socket_t& aGate, ServerGame& aGame);
    bool IsAllClientsReady();
    ~ConnectionManager();
    void Execute();
protected:
private:
    void NewConnection(Network* aNetwork);
    bool mQuit;
    socket_t& mGate;
    ServerGame& mGame;
    std::list< ClientConnection* > mClients;
};

#endif // CONNECTIONMANAGER_H
