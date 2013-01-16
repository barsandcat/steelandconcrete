#include <pch.h>
#include <ConnectionManager.h>

#include <ServerGame.h>
#include <ClientConnection.h>
#include <SSLLogRedirect.h>
#include <openssl/srp.h>
#include <UserList.h>

static int SSLSRPServerParamCallback(SSL *s, int *ad, void *arg)
{
	const char* userName = SSL_get_srp_username(s);

	LOG(INFO) << "User " << userName;

	const User* user = GetUser(userName);

	if (!user)
	{
		LOG(ERROR) << "User " << userName << " doesn't exist";
		*ad = SSL_AD_UNKNOWN_PSK_IDENTITY;
		return SSL3_AL_FATAL;
	}

	SRP_gN *GN = SRP_get_default_gN(FLAGS_srp_default_gN.c_str());
	if(GN == NULL)
	{
		*ad = SSL_AD_INTERNAL_ERROR;
        return SSL3_AL_FATAL;
	}

    if (!SSL_set_srp_server_param(s, GN->N, GN->g, user->GetSalt(), user->GetVerifier(), NULL))
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

    SSL_CTX_set_info_callback(ctx, SSLInfoCallback);
    SSL_CTX_SRP_CTX_init(ctx);
    if (SSL_CTX_set_cipher_list(ctx, "SRP") != 1)
    {
        LOG(ERROR) << "Can not set SRP ciphers";
        return;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    SSL_CTX_set_srp_username_callback(ctx, SSLSRPServerParamCallback);

    AddUser("test", "test");

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
