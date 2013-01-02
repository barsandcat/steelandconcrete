#include <pch.h>
#include <ConnectionManager.h>

#include <ServerGame.h>
#include <ClientConnection.h>

typedef struct srp_server_arg_st
{
	char *expected_user;
	char *pass;
} SRP_SERVER_ARG;

static int ssl_srp_server_param_cb(SSL *s, int *ad, void *arg)
{
	SRP_SERVER_ARG * p = (SRP_SERVER_ARG *) arg;

	LOG(INFO) << "ssl_srp_server_param_cb " << p->expected_user << " " << p->pass << std::endl;

	if (strcmp(p->expected_user, SSL_get_srp_username(s)) != 0)
	{
		LOG(ERROR) << "User %s doesn't exist\n" << SSL_get_srp_username(s);
		return SSL3_AL_FATAL;
	}
	if (SSL_set_srp_server_param_pw(s, p->expected_user, p->pass, "1024") < 0)
	{
		*ad = SSL_AD_INTERNAL_ERROR;
		return SSL3_AL_FATAL;
	}
	return SSL_ERROR_NONE;
}

void ConnectionManager(ServerGame& aGame, Ogre::String aAddress, int32 aPort)
{
    LOG(INFO) << "Init SRP";

    boost::asio::ssl::context sslCtx(boost::asio::ssl::context::tlsv1_server);
    SSL_CTX* ctx = sslCtx.native_handle();

    SSL_CTX_SRP_CTX_init(ctx);
    if (SSL_CTX_set_cipher_list(ctx, "SRP") != 1)
    {
        LOG(ERROR) << "Can not set SRP ciphers";
        return;
    }

    SRP_SERVER_ARG srp_server_arg = {"test", "test"};
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    SSL_CTX_set_srp_cb_arg(ctx, &srp_server_arg);
    SSL_CTX_set_srp_username_callback(ctx, ssl_srp_server_param_cb);

    LOG(INFO) << "Listening to " << aAddress << ":" << aPort;
    boost::asio::io_service IOService;
    boost::asio::ip::tcp::acceptor gate(IOService,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), aPort));

    while (true)
    {
        SSLStreamPtr sslStream(new SSLStream(IOService, sslCtx));
        gate.accept(sslStream->lowest_layer());
        boost::thread thrd(boost::bind(ClientConnection, boost::ref(aGame), sslStream));
    }
}
