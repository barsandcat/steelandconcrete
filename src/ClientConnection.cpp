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
    static_cast< ClientConnection* >(param)->Execute();
}

void ClientConnection::Execute()
{
    mGame.Send(mSocket);

    while (mSocket.is_ok())
    {
        try
        {
            RequestMsg req;
            ReadMessage(mSocket, req);
            if (req.has_type())
            {
                switch (req.type())
                {
                case Disconnect:
                    GetLog() << "Disconnect";
                    mGame.SignalClientEvent();
                    break;
                case Commands:
                    if (req.has_time())
                    {
                        mGame.LoadCommands(req);
                        mLastConfirmedTime = req.time();

                        ResponseMsg rsp;
                        rsp.set_type(Ok);
                        WriteMessage(mSocket, rsp);
                        mGame.SignalClientEvent();

                        GetLog() << "Confirmed time " << req.ShortDebugString() <<  std::endl;
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(NotOk);
                        rsp.set_reason("No time!");
                        WriteMessage(mSocket, rsp);
                    }
                    break;
                case GetTime:
                    if (mGame.GetTime() > mLastConfirmedTime)
                    {
                        ChangeList::Write(mSocket, mGame.GetTime());
                        GetLog() << "Change list send";
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(PleaseWait);
                        WriteMessage(mSocket, rsp);
                    }
                    break;
                }
            }
        }
        catch (std::runtime_error& e)
        {
            GetLog() << e.what();
            mGame.SignalClientEvent();
        }
    }

    mLive = false;
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
