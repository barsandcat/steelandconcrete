#include <pch.h>
#include <ClientConnection.h>

#include <ServerGame.h>
#include <ServerUnit.h>
#include <ServerTile.h>
#include <Network.h>
#include <ChangeList.h>
#include <boost/thread.hpp>
#include <ProtocolVersion.h>
#include <UnitList.h>
#include <ClientFOV.h>
#include <UserList.h>

void ClientConnection(ServerGame& aGame, SSLStreamPtr aSSLStream)
{
    Network network(aSSLStream);
    try
    {
        LOG(INFO) << "SSL handshake";
        aSSLStream->handshake(boost::asio::ssl::stream_base::server);

        PayloadMsg req;
        network.ReadMessage(req);
        LOG(INFO) << "App handshake " << req.ShortDebugString();

        PayloadMsg res;
        res.set_protocolversion(PROTOCOL_VERSION);

        if (req.protocolversion() != PROTOCOL_VERSION)
        {
            res.set_reason("Wrong protocol version");
            network.WriteMessage(res);
            return;
        }

        const char* userName = SSL_get_srp_username(aSSLStream->native_handle());

        LOG(INFO) << "ClientConnection " << userName;

        const User* user = GetUser(userName);
        if (!user)
        {
            res.set_reason("Unknown user!");
            network.WriteMessage(res);
            return;
        }

        ServerUnit* avatarUnit = UnitList::GetUnit(user->GetUnitId());
        res.set_landing_tile(avatarUnit->GetUnitTile().GetTileId());
        res.set_size(aGame.GetSize());
        network.WriteMessage(res);
        LOG(INFO) << "Response " << res.ShortDebugString();

        ClientFOV fov(network, aGame.GetTiles(), user->GetUnitId());

        while (true)
        {
            PayloadMsg req;
            network.ReadMessage(req);
            if (req.has_commandmove())
            {
                const CommandMoveMsg& move = req.commandmove();
                user->GetMind()->SetCommand(*aGame.GetTiles().at(move.position()));
            }
            if (req.has_time())
            {
                boost::shared_lock<boost::shared_mutex> rl(aGame.GetGameMutex());
                fov.SendUpdate(aGame.GetTime(), req.time(), aGame.GetTimeStep(), 6);
                fov.WriteFinalMessage(aGame.GetTime(), aGame.GetUpdateLength());
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
        LOG(INFO) << "ClientConnection exception: " << boost::current_exception_diagnostic_information();
    }
}
