#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sockio.h>
#include <task.h>
#include <Network.h>

class ServerGame;
class ServerUnit;

void task_proc ClientConnectionThreadFunction(void* param);

class ClientConnection
{
public:
    ClientConnection(ServerGame& aGame, Network* aNetwork);
    bool IsLive() const { return mLive; }
    ~ClientConnection();
    void Execute();
protected:
private:
    ServerGame& mGame;
    Network* mNetwork;
    bool mLive;
};

#endif // CLIENTCONNECTION_H
