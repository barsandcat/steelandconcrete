#include <pch.h>

#include <iostream>
#include <string>

#include <Network.h>
#include <Payload.pb.h>
#include <ProtocolVersion.h>

typedef struct srp_client_arg_st
{
    char *srppassin;
    char *srplogin;
} SRP_CLIENT_ARG;

static char* ssl_give_srp_client_pwd_cb(SSL *s, void *arg)
{
    SRP_CLIENT_ARG *srp_client_arg = (SRP_CLIENT_ARG *)arg;
    std::cout << "ssl_give_srp_client_pwd_cb " << srp_client_arg->srplogin << std::endl;
    return BUF_strdup((char *)srp_client_arg->srppassin);
}

int main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);

    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "localhost", "4512");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);


        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv1_client);

        SSL_CTX* ssl_ctx = ctx.native_handle();

        SSL_CTX_SRP_CTX_init(ssl_ctx);

        if (SSL_CTX_set_cipher_list(ssl_ctx, "SRP") != 1)
        {
            std::cout << "SSL_CTX_set_cipher_list failed" << std::endl;
            return 1;
        }

        SRP_CLIENT_ARG srp_client_arg = {"test", "test"};

        if (SSL_CTX_set_srp_username(ssl_ctx, srp_client_arg.srplogin) != 1)
        {
            std::cout << "SSL_CTX_set_srp_username failed" << std::endl;
            return 1;
        }

        SSL_CTX_set_srp_cb_arg(ssl_ctx, &srp_client_arg);
        SSL_CTX_set_srp_client_pwd_callback(ssl_ctx, ssl_give_srp_client_pwd_cb);

        SSLStreamPtr sock( new SSLStream(io_service, ctx));

        boost::asio::connect(sock->lowest_layer(), iterator);
        sock->handshake(boost::asio::ssl::stream_base::client);

        Network* net = new Network(sock);
        LOG(INFO) << "Connected";
        PayloadMsg req;
        req.set_protocolversion(PROTOCOL_VERSION);
        net->WriteMessage(req);

        PayloadMsg res;
        net->ReadMessage(res);
        if(res.has_landing_tile() && res.has_size())
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

