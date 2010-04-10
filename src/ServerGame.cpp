#include <pch.h>
#include <ServerGame.h>

#include <Network.h>
#include <Unit.pb.h>
#include <ServerLog.h>
#include <ConnectionManager.h>
#include <ChangeList.h>
#include <VisualCodes.h>
#include <UpdateTimer.h>

ServerGame::ServerGame(int aSize, int32 aSeaLevel): mGrid(NULL), mUnitCount(0),
    mTime(30), mTimeStep(30),
    mGrass(VC::LIVE | VC::PLANT, 100, 0),
    mZebra(VC::LIVE | VC::ANIMAL | VC::HERBIVORES, 500, 1),
    mAvatar(VC::LIVE | VC::ANIMAL | VC::HUMAN, 999999, 1)
{
    task::initialize(task::normal_stack);
    // Create map
    mGrid = new ServerGeodesicGrid(aSize, aSeaLevel);
    GetLog() << "Size " << aSize << " Tile count " << mGrid->GetTileCount();
    // Populate
    for (size_t i = 0; i < mGrid->GetTileCount(); ++i)
    {
        if (mGrid->GetTile(i).GetHeight() > aSeaLevel)
        {
            switch (rand() % 10)
            {
            case 1:
                CreateUnit(mGrid->GetTile(i), mZebra);
                break;
            case 6:
                CreateUnit(mGrid->GetTile(i), mGrass);
                break;
            }
        }
    }

}

ServerGame::~ServerGame()
{
    delete mGrid;
}

ServerGeodesicGrid& ServerGame::GetGrid()
{
    return *mGrid;
}

void ServerGame::MainLoop(Ogre::String aAddress, Ogre::String aPort)
{
    Ogre::String connection = aAddress + ":" + aPort;
    GetLog() << "Connecting to " << connection;
    socket_t* gate = socket_t::create_global(connection.c_str());
    if (gate->is_ok())
    {
        ConnectionManager manager(*gate, *this);
        UpdateTimer timer(2000);
        while (true)
        {
            timer.Wait();
            UpdateGame();
        }
    }
    else
    {
        GetLog() << "Gate " << GetErrorText(*gate);
    }
    GetLog() << "Game over";
}

ServerUnit& ServerGame::CreateUnit(ServerTile& aTile, const UnitClass& aClass)
{
    ServerUnit* unit = new ServerUnit(aTile, aClass, ++mUnitCount);
    mUnits.insert(std::make_pair(unit->GetUnitId(), unit));
    return *unit;
}

void ServerGame::Send(Network& aNetwork)
{
    critical_section cs(mGameMutex);

    mGrid->Send(aNetwork);

    UnitCountMsg count;
    ServerUnit& avatar = CreateUnit(mGrid->GetTile(rand() % mGrid->GetTileCount()), mAvatar);
    avatar.SetMaster(avatar.GetUnitId());
    count.set_avatar(avatar.GetUnitId());
    count.set_count(mUnits.size());
    count.set_time(mTime);
    aNetwork.WriteMessage(count);
    ServerUnits::const_iterator i = mUnits.begin();
    GetLog() << "Unit count send; " << count.ShortDebugString() ;

    for (; i != mUnits.end(); ++i)
    {
        UnitMsg unit;
        i->second->FillUnitMsg(unit);
        aNetwork.WriteMessage(unit);
    }
    GetLog() << "All units send";
}

void ServerGame::UpdateGame()
{
    critical_section cs(mGameMutex);

    GetLog() << "Update Game!";
    ChangeList::Clear();
    ServerUnits::iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        ServerUnit* unit = i->second;
        assert(unit);
        if (unit->UpdateAgeAndIsTimeToDie(mTimeStep))
        {
            ServerUnits::iterator del = i;
            ++i;
            delete unit;
            mUnits.erase(del);
        }
        else
        {
            unit->ExecuteCommand();
        }
    }
    ChangeList::SetTime(mTime + mTimeStep);
    mTime += mTimeStep;


    GetLog() << "Time: " << mTime;
}


void ServerGame::LoadCommands(const RequestMsg& commands)
{
    critical_section cs(mGameMutex);
    for (int i = 0; i < commands.commands_size(); ++i)
    {
        const CommandMsg& command = commands.commands(i);
        if (command.has_commandmove())
        {
            const CommandMoveMsg& move = command.commandmove();
            ServerUnits::iterator i = mUnits.find(move.unitid());
            if (mUnits.end() != i)
            {
                i->second->SetCommand(mGrid->GetTile(move.position()));
            }
            else
            {
                GetLog() << "Move command for non existing unit! " << command.ShortDebugString();
            }

        }
    }
}
