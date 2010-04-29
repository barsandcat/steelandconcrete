#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sockio.h>
#include <Network.h>

class ServerGame;
class ServerUnit;

class ClientConnection
{
public:
    ClientConnection(ServerGame& aGame, Network* aNetwork);
    void operator()();
protected:
private:
    ServerGame& mGame;
    Network* mNetwork;
};

#endif // CLIENTCONNECTION_H
