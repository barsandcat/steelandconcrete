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
#include <UnitList.h>
#include <UnitListIterator.h>
#include <MindList.h>

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


ServerGame::ServerGame(int aSize):mSize(aSize),
    mGrass(VC::LIVE | VC::PLANT, 100, 0),
    mZebra(VC::LIVE | VC::ANIMAL | VC::HERBIVORES, 500, 1),
    mAvatar(VC::LIVE | VC::ANIMAL | VC::HUMAN, 999999, 1),
    mTimer(2000)
{
    // Create map
    ServerGeodesicGrid grid(mTiles, aSize);
    GetLog() << "Size " << aSize << " Tile count " << mTiles.size();
    GetLog() << "Tile radius " << grid.GetTileRadius();
    // Populate
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        switch (rand() % 10)
        {
        case 1:
            UnitList::NewUnit(*mTiles.at(i), mZebra);
            break;
        case 6:
            UnitList::NewUnit(*mTiles.at(i), mGrass);
            break;
        }
    }

}

ServerGame::~ServerGame()
{
    UnitList::Clear();
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        delete mTiles[i];
        mTiles[i] = NULL;
    }
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

void ServerGame::Send(Network& aNetwork)
{
    GeodesicGridSizeMsg gridInfo;
    gridInfo.set_size(mSize)    ;
    aNetwork.WriteMessage(gridInfo);
    GetLog() << "Grid info sent; " << gridInfo.ShortDebugString() ;
}

void ServerGame::UpdateGame()
{
    boost::lock_guard<boost::shared_mutex> cs(mGameMutex);

    GetLog() << "Update Game!";

    MindList::UpdateMinds(mTimeStep);

    for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
    {
        ServerUnit* unit = i.GetUnit();
        if (unit->UpdateAgeAndIsTimeToDie(mTimeStep))
        {
            UnitList::DeleteUnit(unit->GetUnitId());
        }
        else
        {
            unit->ExecuteCommand();
        }
    }

    mTime += mTimeStep;
    //ChangeList::Commit(mTime);

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
            if (ServerUnit* unit = UnitList::GetUnit(move.unitid()))
            {
                unit->SetCommand(*mTiles.at(move.position()));
            }
            else
            {
                GetLog() << "Move command for non existing unit! " << command.ShortDebugString();
            }

        }
    }
}
