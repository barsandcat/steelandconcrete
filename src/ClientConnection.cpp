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
    mGame.Send(*mNetwork);

    while (mNetwork->IsOk())
    {
        try
        {
            RequestMsg req;
            mNetwork->ReadMessage(req);
            if (req.has_type())
            {
                switch (req.type())
                {
                case REQUEST_DISCONNECT:
                    GetLog() << "Disconnect";
                    mGame.SignalClientEvent();
                    break;
                case REQUEST_COMMANDS:
                    if (req.has_time())
                    {
                        mGame.LoadCommands(req);
                        mLastConfirmedTime = req.time();

                        ResponseMsg rsp;
                        rsp.set_type(RESPONSE_OK);
                        mNetwork->WriteMessage(rsp);
                        GetLog() << "Confirmed time " << req.ShortDebugString();

                        mGame.SignalClientEvent();
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(RESPONSE_NOK);
                        rsp.set_reason("No time!");
                        mNetwork->WriteMessage(rsp);
                    }
                    break;
                case REQUEST_GET_TIME:
                    if (mGame.GetTime() > mLastConfirmedTime)
                    {
                        ChangeList::Write(*mNetwork, mGame.GetTime());
                        GetLog() << "Change list send";
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(RESPONSE_PLEASE_WAIT);
                        mNetwork->WriteMessage(rsp);
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

ClientConnection::ClientConnection(ServerGame& aGame, Network* aNetwork):
        mGame(aGame), mNetwork(aNetwork), mLive(true),
        mLastConfirmedTime(0)
{
    mLastConfirmedTime = mGame.GetTime() - 1;
    task::create(ClientConnectionThreadFunction, this);
}

ClientConnection::~ClientConnection()
{
    delete mNetwork;
    GetLog() << "Socket deletd";
}
