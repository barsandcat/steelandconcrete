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
#include <UnitList.h>
#include <ClientFOV.h>
#include <MindList.h>
#include <Mind.h>

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

        Mind* const avatar = MindList::GetFreeMind();
        if (!avatar)
        {
            res.set_reason("No free mind");
            network.WriteMessage(res);
            return;
        }
        avatar->SetFree(false);

        res.set_avatar(avatar->GetUnitId());
        res.set_size(aGame.GetSize());
        network.WriteMessage(res);
        GetLog() << "Response " << res.ShortDebugString();

        ClientFOV fov(network, aGame, avatar->GetUnitId());

        while (true)
        {
            PayloadMsg req;
            network.ReadMessage(req);
            if (req.has_commandmove())
            {
                const CommandMoveMsg& move = req.commandmove();
                avatar->SetCommand(*aGame.GetTiles().at(move.position()));
            }
            if (req.has_time())
            {
                fov.SendUpdate(req.time());
            }
            else
            {
                PayloadMsg res;
                network.WriteMessage(res);
            }
        }
    }
    catch (...)
    {
        GetLog() << "ClientConnection exception: " << boost::current_exception_diagnostic_information();
    }
}
