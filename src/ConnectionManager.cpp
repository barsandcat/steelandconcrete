#include <pch.h>
#include <ConnectionManager.h>

#include <ServerGame.h>
#include <ClientConnection.h>
#include <SSLLogRedirect.h>
#include <openssl/srp.h>

static int SSLSRPServerParamCallback(SSL *s, int *ad, void *arg)
{
	const char* userName = SSL_get_srp_username(s);

	LOG(INFO) << "User " << userName;

	if (strcmp("test", userName) != 0)
	{
		LOG(ERROR) << "User " << userName << " doesn't exist";
		return SSL3_AL_FATAL;
	}

	SRP_gN *GN = SRP_get_default_gN("1024");
	if(GN == NULL)
	{
		*ad = SSL_AD_INTERNAL_ERROR;
        return SSL3_AL_FATAL;
	}

    const char* hexSalt = "9049B5E776ADB519123C1961AE358D7BE4B12AD0";
    const char* hexVerifier = "82744E847039DD77EA6D296FAFB575790F0394674887D5B991ABB3A49837B039E5D65D6947C0E94C83B0AA8DB100758C297172A03227A90917F2823E7865D88AB0EDA6DF798461DB705D64E5A978FA3DA55667AE4FB3C57D4343203C2F2A03F379DF811EF2955A5CB29F7AA8B97ECCBAB4089A7FB89B4E5CC979CA8E1DAE3034";//BN_bn2hex(verifier);

    LOG(INFO) << "Salt:" << hexSalt << " Verifier:" << hexVerifier;

    BIGNUM *salt = BN_new();
    BIGNUM *verifier = BN_new();

    BN_hex2bn(&salt, hexSalt);
    BN_hex2bn(&verifier, hexVerifier);

    if (!SSL_set_srp_server_param(s, GN->N, GN->g, salt, verifier, NULL))
    {
        *ad = SSL_AD_INTERNAL_ERROR;
        return SSL3_AL_FATAL;
    }

    BN_clear_free(salt);
    BN_clear_free(verifier);

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
