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

    try
    {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "localhost", "4512");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        SocketSharedPtr sock(new tcp::socket(io_service));
        sock->connect(*iterator);

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
        return 0;
    }
    catch(...)
    {
        GetLog() << boost::current_exception_diagnostic_information();
        return 1;
    }

}

