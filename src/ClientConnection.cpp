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

DEFINE_int32(vision_range, 6, "Radius (in tiles) around player to send over network");

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

        res.set_avatar(user->GetUnitId());
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
                const int32 toSend = (aGame.GetTime() - req.time()) / FLAGS_time_step;
                const bool outOfBounds = req.time() <= 0 || toSend >= FLAGS_max_change_list_size;
                if (outOfBounds)
                {
                    fov.WriteFullUpdate(FLAGS_vision_range);
                }
                else
                {
                    fov.WritePartialUpdate(toSend, FLAGS_vision_range);
                }
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
