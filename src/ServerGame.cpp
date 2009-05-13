#include <ServerGame.h>
#include <sockio.h>
#include <task.h>
#include <Network.h>
#include <Unit.pb.h>
#include <ServerLog.h>
#include <ConnectionManager.h>

ServerGame::ServerGame(): mGrid(NULL), mUnitCount(0)
{
    task::initialize(task::normal_stack);
    mGrid = new ServerGeodesicGrid(2);
}

ServerGame::~ServerGame()
{
    delete mGrid;
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
        while(true)
        {
            if (manager.IsAllClientsReady())
            {
                UpdateUnits();
                GetLog() << "Game!" << std::endl;
            }
            sleep(3);
        }
    }
    GetLog() << "Game over";
}

ServerUnit& ServerGame::CreateUnit(ServerTile& mTile)
{
    ServerUnit* unit = new ServerUnit(mTile, ++mUnitCount);
    mUnits.insert(std::make_pair(unit->GetIndex(), unit));
    return *unit;
}

void ServerGame::Send(socket_t& aSocket) const
{
    mGrid->Send(aSocket);

    UnitCountMsg count;
    count.set_count(mUnits.size());
    WriteMessage(aSocket, count);
    ServerUnits::const_iterator i = mUnits.begin();
    GetLog() << "Unit count send" << std::endl;

    for (;i != mUnits.end(); ++i)
    {
        UnitMsg unit;
        unit.set_tag(i->first);
        unit.set_tile(i->second->GetTile().GetIndex());
        WriteMessage(aSocket, unit);
    }
    GetLog() << "All units send" << std::endl;
}

void ServerGame::UpdateUnits()
{
    ServerUnits::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        i->second->Update();
    }
}
