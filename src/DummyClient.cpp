#include <pch.h>
#include <iostream>
#include <string>

#include <Network.h>
#include <Payload.pb.h>
#include <ProtocolVersion.h>

int main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);

    try
    {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "localhost", "4512");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        SocketSharedPtr sock(new tcp::socket(io_service));
        sock->connect(*iterator);

        Network* net = new Network(sock);
        LOG(INFO) << "Connected";
        PayloadMsg req;
        req.set_protocolversion(PROTOCOL_VERSION);
        net->WriteMessage(req);

        PayloadMsg res;
        net->ReadMessage(res);
        if(res.has_avatar() && res.has_size())
        {
            int64 mTime = 0;

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
                    LOG(INFO) << "REQUEST_GET_TIME";

                    net->ReadMessage(rsp);
                    while(!rsp.last())
                    {
                        LOG(INFO) << "Changes " << rsp.changes_size();
                        rsp.Clear();
                        net->ReadMessage(rsp);
                    }
                    mTime = rsp.time();
                    updateLength = rsp.update_length();
                    LOG(INFO) << "New time " << mTime << " next update in " << updateLength;
                }
                catch(std::exception& e)
                {
                    LOG(INFO) << "Main loop crash: " << e.what();
                    return 1;
                }
            }
        }
        else
        {
            delete net;
            LOG(INFO) << "Server rejected connection";
            google::ShutdownGoogleLogging();
            return 2;
        }
        google::ShutdownGoogleLogging();
        return 0;
    }
    catch(...)
    {
        LOG(INFO) << boost::current_exception_diagnostic_information();
        google::ShutdownGoogleLogging();
        return 1;
    }

}

