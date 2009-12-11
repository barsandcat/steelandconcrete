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
    GameTime GetLastConfirmedTime() const { return mLastConfirmedTime; }
    ~ClientConnection();
    void Execute();
protected:
private:
    ServerGame& mGame;
    Network* mNetwork;
    bool mLive;
    GameTime mLastConfirmedTime;
};

#endif // CLIENTCONNECTION_H
