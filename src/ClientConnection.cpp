#include <pch.h>
#include <ClientConnection.h>

#include <ServerGame.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <ServerLog.h>
#include <Network.h>
#include <ChangeList.h>
#include <boost/thread.hpp>
#include <ProtocolVersion.h>
#include <Avatar.h>
#include <UnitList.h>
#include <ClientFOV.h>

void ClientConnection(ServerGame& aGame, SocketSharedPtr aSocket)
{
    Network network(aSocket);
    try
    {
        PayloadMsg req;
        network.ReadMessage(req);
        GetLog() << "Request " << req.ShortDebugString();

        PayloadMsg res;
        res.set_protocolversion(ProtocolVersion);

        if (req.protocolversion() != ProtocolVersion)
        {
            res.set_reason("Wrong protocol version");
            network.WriteMessage(res);
            return;
        }

        Avatar avatar(aGame);

        res.set_avatar(avatar.GetId());
        res.set_size(aGame.GetSize());
        network.WriteMessage(res);
        GetLog() << "Response " << res.ShortDebugString();

        ClientFOV fov(network, aGame, avatar.GetId());

        while (true)
        {
            PayloadMsg req;
            network.ReadMessage(req);
            if (req.has_commandmove())
            {
                aGame.LoadCommand(avatar.GetId(), req);
            }
            if (req.has_time())
            {
                fov.SendUpdate(req.time());
            }
        }
    }
    catch (...)
    {
        GetLog() << boost::current_exception_diagnostic_information();
    }
}
