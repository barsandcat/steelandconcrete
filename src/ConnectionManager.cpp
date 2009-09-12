#include <pch.h>
#include <ConnectionManager.h>

#include <ServerLog.h>
#include <ServerGame.h>

void task_proc ManagerThreadFunction(void *param)
{
    static_cast< ConnectionManager* >(param)->Execute();
}

void ConnectionManager::Execute()
{
    mQuit = !mGate.is_ok();
    GetLog() << "Gate " << (mQuit ? "not opened" : "opened");
    while (!mQuit)
    {
        socket_t* clientSocket = mGate.accept();
        if (clientSocket->is_ok())
            NewConnection(*clientSocket);
        mQuit = !mGate.is_ok();
    }
}

ConnectionManager::ConnectionManager(socket_t& aGate, ServerGame& aGame):
    mQuit(false), mGate(aGate), mGame(aGame)
{
    task::create(ManagerThreadFunction, this);
}

void ConnectionManager::NewConnection(socket_t& aSocket)
{
    mClients.push_back(new ClientConnection(mGame, aSocket));
}

ConnectionManager::~ConnectionManager()
{
    delete &mGate;
}

bool ConnectionManager::IsAllClientsReady()
{
    bool allReady = !mClients.empty();
    std::list< ClientConnection* >::iterator i = mClients.begin();
    while(i != mClients.end())
    {
        if ((*i)->IsLive())
        {
            allReady = allReady && ((*i)->GetLastConfirmedTime() == mGame.GetTime());
            ++i;
        }
        else
        {
            delete *i;
            i = mClients.erase(i);
        }
    }
    return allReady;
}
