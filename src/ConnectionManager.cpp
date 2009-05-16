#include <ConnectionManager.h>
#include <ServerLog.h>
#include <task.h>

void task_proc ManagerThreadFunction(void *param)
{
    ConnectionManager& self = *(static_cast< ConnectionManager* >(param));
    self.mQuit = !self.mGate.is_ok();
    GetLog() << "Gate " << (self.mQuit ? "not opened" : "opened") << std::endl;
    while (!self.mQuit)
    {
        socket_t* clientSocket = self.mGate.accept();
        if (clientSocket->is_ok())
            self.NewConnection(*clientSocket);
        self.mQuit = !self.mGate.is_ok();
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
            allReady = allReady && (*i)->IsReady();
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
