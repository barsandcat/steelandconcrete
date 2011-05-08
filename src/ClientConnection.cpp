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
#include <Avatar.h>
#include <UnitList.h>
#include <ClientFOV.h>

void ClientConnection(ServerGame& aGame, SocketSharedPtr aSocket)
{
    Network network(aSocket);
    try
    {
        ConnectionRequestMsg req;
        network.ReadMessage(req);
        GetLog() << "Request " << req.ShortDebugString();

        ConnectionResponseMsg res;
        res.set_protocolversion(ProtocolVersion);

        if (req.protocolversion() != ProtocolVersion)
        {
            res.set_result(CONNECTION_WRONG_VERSION);
            network.WriteMessage(res);
            return;
        }

        Avatar avatar(aGame);

        res.set_result(CONNECTION_ALLOWED);
        res.set_avatar(avatar.GetId());
        res.set_size(aGame.GetSize());
        network.WriteMessage(res);
        GetLog() << "Response " << res.ShortDebugString();

        ClientFOV fov(network, aGame, avatar.GetId());

        while (true)
        {
            RequestMsg req;
            network.ReadMessage(req);
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
                        fov.SendUpdate(req.time());
                    }
                    else
                    {
                        ResponseMsg rsp;
                        rsp.set_type(RESPONSE_NOK);
                        rsp.set_reason("No time!");
                        network.WriteMessage(rsp);
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
