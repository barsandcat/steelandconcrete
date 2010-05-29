#include <pch.h>
#include <ServerGame.h>

#include <Network.h>
#include <Unit.pb.h>
#include <ServerLog.h>
#include <ConnectionManager.h>
#include <ChangeList.h>
#include <VisualCodes.h>
#include <UpdateTimer.h>
#include <boost/thread.hpp>

GameTime ServerGame::mTime = 1;
GameTime ServerGame::mTimeStep = 1;

GameTime ServerGame::GetTime()
{
    return mTime;
}

GameTime ServerGame::GetTimeStep()
{
    return mTimeStep;
}


ServerGame::ServerGame(int aSize, int32 aSeaLevel): mGrid(NULL), mUnitCount(0),
    mGrass(VC::LIVE | VC::PLANT, 100, 0),
    mZebra(VC::LIVE | VC::ANIMAL | VC::HERBIVORES, 500, 1),
    mAvatar(VC::LIVE | VC::ANIMAL | VC::HUMAN, 999999, 1),
    mTimer(2000)
{
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

void ServerGame::MainLoop(Ogre::String aAddress, int32 aPort)
{
    GetLog() << "Connecting to " << aAddress << ":" << aPort;

    boost::thread connectionManager(ConnectionManager, boost::ref(*this), aAddress, aPort);

    while (true)
    {
        GetLog() << "Whait...";
        mTimer.Wait();
        UpdateGame();
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
    boost::shared_lock<boost::shared_mutex> cs(mGameMutex);

    mGrid->Send(aNetwork);

    UnitCountMsg count;
    count.set_avatar(0);
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
    boost::lock_guard<boost::shared_mutex> cs(mGameMutex);

    GetLog() << "Update Game!";

    std::vector<UnitId> mDeleteList;
    mDeleteList.resize(mUnits.size() * 0.1f);

    for (ServerUnits::iterator i = mUnits.begin(); i != mUnits.end(); ++i)
    {
        ServerUnit* unit = i->second;
        assert(unit);
        if (unit->UpdateAgeAndIsTimeToDie(mTimeStep))
        {
            mDeleteList.push_back(i->first);
        }
        else
        {
            unit->ExecuteCommand();
        }
    }

    // Remove deleted units
    for (size_t i = 0; i < mDeleteList.size(); ++i)
    {
        ServerUnit* unit = mUnits[mDeleteList[i]];
        mUnits.erase(mDeleteList[i]);
        delete unit;
    }

    mTime += mTimeStep;
    ChangeList::Commit(mTime);

    GetLog() << "Time: " << mTime;
}


void ServerGame::LoadCommands(const RequestMsg& commands)
{
    boost::lock_guard<boost::shared_mutex> cs(mGameMutex);
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
