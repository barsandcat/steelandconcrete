#include <pch.h>
#include <ConnectionManager.h>

#include <ServerGame.h>
#include <ClientConnection.h>

void ConnectionManager(ServerGame& aGame, Ogre::String aAddress, int32 aPort)
{
    LOG(INFO) << "Listening to " << aAddress << ":" << aPort;

    boost::asio::io_service mIOService;
    tcp::acceptor gate(mIOService, tcp::endpoint(tcp::v4(), aPort));
    while (true)
    {
        SocketSharedPtr clientSocket(new tcp::socket(mIOService));
        gate.accept(*clientSocket);
        boost::thread thrd(boost::bind(ClientConnection, boost::ref(aGame), clientSocket));
    }
}
