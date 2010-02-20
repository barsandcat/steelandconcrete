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
#include <task.h>

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

            bool mTurnDone = false;
            while(true)
            {
				task::sleep(1000);
                try
                {
                    RequestMsg req;
                    ResponseMsg rsp;
                    if(!mTurnDone)
                    {
                        req.set_type(REQUEST_COMMANDS);
                        req.set_time(mTime);
                        req.set_last(true);
                        net->WriteMessage(req);
                        GetLog() << "REQUEST_COMMANDS " << mTime;
                        net->ReadMessage(rsp);
                        GetLog() << rsp.ShortDebugString();
                        mTurnDone = true;

                    }
                    else
                    {
                        req.set_type(REQUEST_GET_TIME);
                        net->WriteMessage(req);
                        GetLog() << "REQUEST_GET_TIME";

                        net->ReadMessage(rsp);
                        switch(rsp.type())
                        {
                        case RESPONSE_CHANGES:
                            while(!rsp.last())
                            {
                                rsp.Clear();
                                net->ReadMessage(rsp);
                            }
                            mTime = rsp.time();
                            GetLog() << "New time " << mTime;
                            mTurnDone = false;
                            break;
                        default:
                            break;
                        }
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

