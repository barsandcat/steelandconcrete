#include <pch.h>
#include <ConnectionManager.h>

#include <ServerLog.h>
#include <ServerGame.h>
#include <Handshake.pb.h>
#include <Network.h>
#include <ProtocolVersion.h>

void task_proc ManagerThreadFunction(void *param)
{
    ConnectionManager* cm = static_cast< ConnectionManager* >(param);
		(*cm)();
}

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
                NewConnection(net);
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
}

ConnectionManager::ConnectionManager(socket_t& aGate, ServerGame& aGame):
    mQuit(false), mGate(aGate), mGame(aGame)
{
    task::create(ManagerThreadFunction, this);
}

void ConnectionManager::NewConnection(Network* aNetwork)
{
    mClients.push_back(new ClientConnection(mGame, aNetwork));
}

ConnectionManager::~ConnectionManager()
{
    delete &mGate;
}

