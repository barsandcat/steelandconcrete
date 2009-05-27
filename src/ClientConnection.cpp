#include <ClientConnection.h>
#include <ServerGame.h>
#include <task.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <ServerLog.h>
#include <Request.pb.h>
#include <Network.h>
#include <Response.pb.h>
#include <stdexcept>

void task_proc ClientConnectionThreadFunction(void *param)
{
    ClientConnection& self = *(static_cast< ClientConnection* >(param));
    self.mGame.Send(self.mSocket);

    while (self.mSocket.is_ok())
    {
        task::sleep(300);
        try
        {
            RequestMsg req;
            ReadMessage(self.mSocket, req);
            if (req.has_type())
            {
                switch (req.type())
                {
                case Disconnect:
                    GetLog() << "Disconnect" << std::endl;
                    break;
                case ConfirmTime:
                    if (req.has_time())
                    {
                        self.mLastConfirmedTime = req.time();

                        ResponseMsg rsp;
                        rsp.set_type(Ok);
                        WriteMessage(self.mSocket, rsp);

                        GetLog() << "Confirmed time " << req.ShortDebugString() <<  std::endl;
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(NotOk);
                        rsp.set_reason("No time!");
                        WriteMessage(self.mSocket, rsp);
                    }
                    break;
                case GetTime:
                    if (self.mGame.GetTime() > self.mLastConfirmedTime)
                    {
                        ResponseMsg rsp;
                        rsp.set_type(NewTime);
                        rsp.set_time(self.mGame.GetTime());
                        WriteMessage(self.mSocket, rsp);
                        GetLog() << "New time send " << rsp.ShortDebugString() << std::endl;
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(PleaseWait);
                        WriteMessage(self.mSocket, rsp);
                    }
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

ClientConnection::ClientConnection(ServerGame& aGame, socket_t& aSocket):
        mGame(aGame), mSocket(aSocket), mLive(true),
        mLastConfirmedTime(0)
{
    mLastConfirmedTime = mGame.GetTime() - 1;
    task::create(ClientConnectionThreadFunction, this);
}

ClientConnection::~ClientConnection()
{
    mSocket.close();
    delete &mSocket;
    GetLog() << "Socket deletd" << std::endl;
}
