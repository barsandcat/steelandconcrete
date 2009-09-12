#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sockio.h>
#include <task.h>
class ServerGame;
class ServerUnit;

void task_proc ClientConnectionThreadFunction(void* param);

class ClientConnection
{
public:
    ClientConnection(ServerGame& aGame, socket_t& aSocket);
    bool IsLive() const { return mLive; }
    GameTime GetLastConfirmedTime() const { return mLastConfirmedTime; }
    ~ClientConnection();
    void Execute();
protected:
private:

    ServerGame& mGame;
    socket_t& mSocket;
    bool mLive;
    GameTime mLastConfirmedTime;
};

#endif // CLIENTCONNECTION_H
