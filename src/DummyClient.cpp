#include <pch.h>
#include <iostream>
#include <string>

#include <Network.h>
#include <ServerLog.h>
#include <Payload.pb.h>
#include <ProtocolVersion.h>

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
        PayloadMsg req;
        req.set_protocolversion(ProtocolVersion);
        net->WriteMessage(req);

        PayloadMsg res;
        net->ReadMessage(res);
        if(res.has_avatar() && res.has_size())
        {
            int mTime = 0;

            int32 updateLength = 1000;
            while(true)
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(updateLength));
                try
                {
                    PayloadMsg req;
                    PayloadMsg rsp;
                    req.set_time(mTime);
                    req.set_last(true);
                    net->WriteMessage(req);
                    GetLog() << "REQUEST_GET_TIME";

                    net->ReadMessage(rsp);
                    while(!rsp.last())
                    {
                        GetLog() << "Changes " << rsp.changes_size();
                        rsp.Clear();
                        net->ReadMessage(rsp);
                    }
                    mTime = rsp.time();
                    updateLength = rsp.update_length();
                    GetLog() << "New time " << mTime << " next update in " << updateLength;
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

