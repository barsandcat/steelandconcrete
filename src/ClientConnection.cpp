#include <pch.h>
#include <ClientConnection.h>

#include <ServerGame.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <ServerLog.h>
#include <Request.pb.h>
#include <Network.h>
#include <Response.pb.h>
#include <ChangeList.h>

void task_proc ClientConnectionThreadFunction(void *param)
{
    ClientConnection& self = *(static_cast< ClientConnection* >(param));
    self.mGame.Send(self.mSocket);

    while (self.mSocket.is_ok())
    {
        try
        {
            RequestMsg req;
            ReadMessage(self.mSocket, req);
            if (req.has_type())
            {
                switch (req.type())
                {
                case Disconnect:
                    GetLog() << "Disconnect";
                    self.mGame.SignalClientEvent();
                    break;
                case Commands:
                    if (req.has_time())
                    {
                        self.mGame.LoadCommands(req);
                        self.mLastConfirmedTime = req.time();

                        ResponseMsg rsp;
                        rsp.set_type(Ok);
                        WriteMessage(self.mSocket, rsp);
                        self.mGame.SignalClientEvent();

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
                        ChangeList::Write(self.mSocket, self.mGame.GetTime());
                        GetLog() << "Change list send";
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
            GetLog() << e.what();
            self.mGame.SignalClientEvent();
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
    GetLog() << "Socket deletd";
}
