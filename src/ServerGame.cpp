#include <pch.h>
#include <ServerGame.h>

#include <Network.h>
#include <Unit.pb.h>
#include <ServerLog.h>
#include <ConnectionManager.h>
#include <ChangeList.h>

ServerGame::ServerGame(): mGrid(NULL), mUnitCount(0), mTime(30), mTimeStep(30)
{
    task::initialize(task::normal_stack);
    mClientEvent = new event();
    mGrid = new ServerGeodesicGrid(4);
    for (size_t i = 0; i < 15; ++i)
    {
        CreateUnit(mGrid->GetTile(rand() % mGrid->GetTileCount()));
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

ServerUnit& ServerGame::CreateUnit(ServerTile& aTile)
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
    GetLog() << "Unit count send";

    for (;i != mUnits.end(); ++i)
    {
        UnitMsg unit;
        unit.set_tag(i->first);
        unit.set_tile(i->second->GetPosition().GetTileId());
        WriteMessage(aSocket, unit);
    }
    GetLog() << "All units send";
}

void ServerGame::UpdateGame()
{
    GetLog() << "Update Game!";
    ChangeList::Clear();
    ServerUnits::iterator i = mUnits.begin();
    mTime += mTimeStep;
    for (; i != mUnits.end(); ++i)
    {
        ServerUnit& unit = *i->second;
        unit.ExecuteCommand();
    }

    GetLog() << "Time: " << mTime;
}


void ServerGame::LoadCommands(const RequestMsg& commands)
{
    for (int i = 0; i < commands.commands_size(); ++i)
    {
        const CommandMsg& command = commands.commands(i);
        if (command.has_commandmove())
        {
            const CommandMoveMsg& move = command.commandmove();
            mUnits[move.unitid()]->SetCommand(mGrid->GetTile(move.position()));
        }
    }
}
