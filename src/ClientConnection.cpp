#include <ClientConnection.h>
#include <ServerGame.h>
#include <task.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <ServerLog.h>

void task_proc ClientConnectionThreadFunction(void *param)
{
    ClientConnection& self = *(static_cast< ClientConnection* >(param));
    self.mGame.Send(self.mSocket);
    self.mReady = false;

    while (self.mSocket.is_ok())
    {
        task::sleep(300);
        GetLog() << "Client!" << std::endl;
    }

    self.mLive = false;
}

ClientConnection::ClientConnection(ServerGame& aGame, socket_t& aSocket):mGame(aGame), mSocket(aSocket), mLive(true), mReady(false)
{
    task::create(ClientConnectionThreadFunction, this);
}

ClientConnection::~ClientConnection()
{
    mSocket.close();
    delete &mSocket;
    GetLog() << "Socket deletd" << std::endl;
}
