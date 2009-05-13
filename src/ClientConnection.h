#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sockio.h>
class ServerGame;
class ServerUnit;

class ClientConnection
{
public:
    ClientConnection(ServerGame& aGame, socket_t& aSocket);
    bool IsLive() const { return mLive; }
    bool IsReady() const { return mReady; }
    ~ClientConnection();
protected:
private:
    friend void ClientConnectionThreadFunction(void* param);
    ServerGame& mGame;
    socket_t& mSocket;
    ServerUnit* mAvatar;
    bool mLive;
    bool mReady;
};

#endif // CLIENTCONNECTION_H
