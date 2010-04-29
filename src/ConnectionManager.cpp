#include <pch.h>
#include <ConnectionManager.h>

#include <ServerLog.h>
#include <ServerGame.h>
#include <Handshake.pb.h>
#include <Network.h>
#include <ProtocolVersion.h>

void ConnectionManager::operator()()
{
    mQuit = !mGate.is_ok();
    GetLog() << "Gate " << (mQuit ? "not opened" : "opened");
    while (!mQuit)
    {
        socket_t* clientSocket = mGate.accept();
        if (clientSocket->is_ok())
        {
            Network* net = new Network(clientSocket);
            ConnectionRequestMsg req;
            net->ReadMessage(req);
            GetLog() << "Client request " << req.ShortDebugString();

            ConnectionResponseMsg res;
            res.set_protocolversion(ProtocolVersion);
            if (req.protocolversion() == ProtocolVersion)
            {
                res.set_result(CONNECTION_ALLOWED);
                net->WriteMessage(res);
                boost::thread thrd(ClientConnection(mGame, net));
            }
            else
            {
                res.set_result(CONNECTION_WRONG_VERSION);
                net->WriteMessage(res);
                delete net;
            }
        }
        mQuit = !mGate.is_ok();
    }
    delete &mGate;
}

ConnectionManager::ConnectionManager(socket_t& aGate, ServerGame& aGame):
    mQuit(false), mGate(aGate), mGame(aGame)
{
}

