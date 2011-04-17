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

void AddShowTile(ResponseMsg& aResponse, TileId aTileId)
{
    ChangeMsg* change = aResponse.add_changes();
    ShowTileMsg* showTile = change->mutable_showtile();
    showTile->set_tileid(aTileId);
}



void ClientConnection(ServerGame& aGame, SocketSharedPtr aSocket)
{
    Network network(aSocket);
    try
    {
        ConnectionRequestMsg req;
        network.ReadMessage(req);
        GetLog() << "Client request " << req.ShortDebugString();

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
        network.WriteMessage(res);
        GetLog() << "Client response " << res.ShortDebugString();

        aGame.Send(network);


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
                        // show tiles
                        ResponseMsg response;
                        response.set_type(RESPONSE_PART);
                        ServerTile& tile = UnitList::GetUnit(avatar.GetId())->GetPosition();
                        AddShowTile(response, tile.GetTileId());
                        for (size_t n = 0; n < tile.GetNeighbourCount(); ++n)
                        {
                            AddShowTile(response, tile.GetNeighbour(n).GetTileId());
                        }
                        network.WriteMessage(response);
                        // set time
                        ResponseMsg emptyMsg;
                        emptyMsg.set_type(RESPONSE_OK);
                        emptyMsg.set_time(ServerGame::GetTime());
                        emptyMsg.set_update_length(aGame.GetUpdateLength());
                        network.WriteMessage(emptyMsg);

                        //ChangeList::Write(network, req.time(), aGame.GetUpdateLength());
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
