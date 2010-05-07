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
#include <boost/thread.hpp>
#include <Handshake.pb.h>
#include <ProtocolVersion.h>


void ClientConnection(ServerGame& aGame, SocketSharedPtr aSocket)
{
    try
    {
        Network mNetwork(aSocket);

        ConnectionRequestMsg req;
        mNetwork.ReadMessage(req);
        GetLog() << "Client request " << req.ShortDebugString();

        ConnectionResponseMsg res;
        res.set_protocolversion(ProtocolVersion);
        if (req.protocolversion() == ProtocolVersion)
        {
            res.set_result(CONNECTION_ALLOWED);
            mNetwork.WriteMessage(res);
        }
        else
        {
            res.set_result(CONNECTION_WRONG_VERSION);
            mNetwork.WriteMessage(res);
            return;
        }

        aGame.Send(mNetwork);


        while (true)
        {
            RequestMsg req;
            mNetwork.ReadMessage(req);
            if (req.has_type())
            {
                switch (req.type())
                {
                case REQUEST_DISCONNECT:
                    break;
                case REQUEST_GET_TIME:
                    if (req.has_time())
                    {
                        aGame.LoadCommands(req);
                        ChangeList::Write(mNetwork, req.time(), aGame.GetUpdateLength());
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(RESPONSE_NOK);
                        rsp.set_reason("No time!");
                        mNetwork.WriteMessage(rsp);
                    }
                    break;
                }
            }
        }
    }
    catch (...)
    {
        GetLog() << boost::current_exception_diagnostic_information();
    }
}
