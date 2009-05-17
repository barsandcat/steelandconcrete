#include <ClientConnection.h>
#include <ServerGame.h>
#include <task.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <ServerLog.h>
#include <Request.pb.h>
#include <Network.h>
#include <stdexcept>

void task_proc ClientConnectionThreadFunction(void *param)
{
    ClientConnection& self = *(static_cast< ClientConnection* >(param));
    self.mGame.Send(self.mSocket);
    self.mReady = false;

    while (self.mSocket.is_ok())
    {
        task::sleep(300);
        try
        {
            RequestMsg msg;
            ReadMessage(self.mSocket, msg);
            if (msg.has_type())
            {
                switch (msg.type())
                {
                case Disconnect:
                    GetLog() << "Disconnect" << std::endl;
                    break;
                case Ready:
                    GetLog() << "Ready" << std::endl;
                    self.mReady = true;
                    break;
                }
            }

        }
        catch (std::runtime_error& e)
        {
            GetLog() << e.what() << std::endl;
        }
    }

    self.mLive = false;
}

ClientConnection::ClientConnection(ServerGame& aGame, socket_t& aSocket): mGame(aGame), mSocket(aSocket), mLive(true), mReady(false)
{
    task::create(ClientConnectionThreadFunction, this);
}

ClientConnection::~ClientConnection()
{
    mSocket.close();
    delete &mSocket;
    GetLog() << "Socket deletd" << std::endl;
}
