#include <pch.h>
#include <iostream>
#include <string>

#include <Network.h>
#include <ServerLog.h>
#include <Handshake.pb.h>
#include <ProtocolVersion.h>
#include <GeodesicGrid.pb.h>
#include <Unit.pb.h>
#include <Request.pb.h>
#include <Response.pb.h>

int main()
{
    std::string connection = "localhost:4512";
    socket_t* sock = socket_t::connect(connection.c_str(), socket_t::sock_global_domain, 3, 1);
    if(sock->is_ok())
    {
        Network* net = new Network(sock);
        GetLog() << "Connected";
        ConnectionRequestMsg req;
        req.set_protocolversion(ProtocolVersion);
        net->WriteMessage(req);

        ConnectionResponseMsg res;
        net->ReadMessage(res);
        if(res.result() == CONNECTION_ALLOWED)
        {
            GeodesicGridSizeMsg gridInfo;
            net->ReadMessage(gridInfo);

            GetLog() << "Recived grid info " << gridInfo.ShortDebugString();

            for(size_t i = 0; i < gridInfo.tilecount();)
            {
                TileListMsg tiles;
                net->ReadMessage(tiles);
                i += tiles.tiles_size();
            }
            GetLog() << "Recived all tiles";

            for(size_t i = 0; i < gridInfo.edgecount();)
            {
                EdgeListMsg edges;
                net->ReadMessage(edges);
                i += edges.edges_size();
            }
            GetLog() << "Recived all edges ";

            UnitCountMsg unitCount;
            net->ReadMessage(unitCount);
            int mTime = unitCount.time();
            GetLog() << "Recived unit count " << unitCount.ShortDebugString();

            for(size_t i = 0; i < unitCount.count(); ++i)
            {
                UnitMsg unit;
                net->ReadMessage(unit);
            }

			int32 updateLength = 1000;
            while(true)
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(updateLength));
                try
                {
                    RequestMsg req;
                    ResponseMsg rsp;
                    req.set_type(REQUEST_GET_TIME);
                    req.set_time(mTime);
                    req.set_last(true);
                    net->WriteMessage(req);
                    GetLog() << "REQUEST_GET_TIME";

                    net->ReadMessage(rsp);
                    switch(rsp.type())
                    {
                    case RESPONSE_PART:
                    case RESPONSE_OK:
                        while(rsp.type() != RESPONSE_OK)
                        {
							GetLog() << "Changes " << rsp.changes_size();
                            rsp.Clear();
                            net->ReadMessage(rsp);
                        }
                        mTime = rsp.time();
						updateLength = rsp.update_length();
                        GetLog() << "New time " << mTime << " next update in " << updateLength;
                        break;
                    default:
                        break;
                    }
                }
                catch(std::exception& e)
                {
                    GetLog() << "Main loop crash: " << e.what();
                    return 1;
                }
            }
        }
        else
        {
            delete net;
            GetLog() << "Server rejected connection";
            return 2;
        }
    }
    else
    {
        GetLog() << "No server! " << GetErrorText(*sock);
        delete sock;
        return 3;
    }
    return 0;
}

