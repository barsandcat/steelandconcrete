#include <ServerGame.h>
#include <sockio.h>
#include <task.h>
#include <Network.h>
#include <Unit.pb.h>
#include <ServerLog.h>
#include <ConnectionManager.h>

ServerGame::ServerGame(): mGrid(NULL), mUnitCount(0), mTime(1)
{
    task::initialize(task::normal_stack);
    mClientEvent = new event();
    mGrid = new ServerGeodesicGrid(2);
    for (size_t i = 0; i < 15; ++i)
    {
        CreateUnit(rand() % mGrid->GetTileCount());
    }
}

ServerGame::~ServerGame()
{
    delete mGrid;
    delete mClientEvent;
}

ServerGeodesicGrid& ServerGame::GetGrid()
{
    return *mGrid;
}

void ServerGame::MainLoop()
{
    socket_t* gate = socket_t::create_local("localhost:4512");
    if (gate->is_ok())
    {
        ConnectionManager manager(*gate, *this);
        while (true)
        {
            mClientEvent->wait();
            mClientEvent->reset();
            if (manager.IsAllClientsReady())
            {
                UpdateGame();
            }
        }
    }
    GetLog() << "Game over";
}

ServerUnit& ServerGame::CreateUnit(TileId aTile)
{
    ServerUnit* unit = new ServerUnit(aTile, ++mUnitCount);
    mUnits.insert(std::make_pair(unit->GetUnitId(), unit));
    return *unit;
}

void ServerGame::Send(socket_t& aSocket) const
{
    mGrid->Send(aSocket);

    UnitCountMsg count;
    count.set_count(mUnits.size());
    count.set_time(mTime);
    WriteMessage(aSocket, count);
    ServerUnits::const_iterator i = mUnits.begin();
    GetLog() << "Unit count send" << std::endl;

    for (;i != mUnits.end(); ++i)
    {
        UnitMsg unit;
        unit.set_tag(i->first);
        unit.set_tile(i->second->GetPosition());
        WriteMessage(aSocket, unit);
    }
    GetLog() << "All units send" << std::endl;
}

void ServerGame::UpdateGame()
{
    GetLog() << "Update Game!" << std::endl;
    ServerUnits::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        i->second->Update();
    }
    ++mTime;
    GetLog() << "Time: " << mTime << std::endl;
}
