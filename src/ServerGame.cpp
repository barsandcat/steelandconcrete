#include <pch.h>
#include <ServerGame.h>

#include <Network.h>
#include <ConnectionManager.h>
#include <ChangeList.h>
#include <VisualCodes.h>
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

void SpreadHeight(ServerTile& aTile, int32 aHeight)
{
    if (aTile.GetHeight() > 0 || aHeight <= 1)
    {
        return;
    }

    aTile.SetHeight(rand() % aHeight);

    for (size_t i = 0; i < aTile.GetNeighbourCount(); ++i)
    {
        SpreadHeight(aTile.GetNeighbour(i), aHeight * 0.5);
    }
}


ServerGame::ServerGame(int aSize):mSize(aSize),
    mGrass(VC::LIVE | VC::PLANT, 100, 0),
    mZebra(VC::LIVE | VC::ANIMAL | VC::HERBIVORES, 500, 1),
    mAvatar(VC::LIVE | VC::ANIMAL | VC::HUMAN, 999999, 1),
    mTimer(2000)
{
    // Create map
    ServerGeodesicGrid grid(mTiles, aSize);
    LOG(INFO) << "Size " << aSize << " Tile count " << mTiles.size();
    LOG(INFO) << "Tile radius " << grid.GetTileRadius();

    // Generate height
    SpreadHeight(*mTiles.at(2), 10000);
    SpreadHeight(*mTiles.at(4), 5000);

    // Populate
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        ServerTile& tile = *mTiles.at(i);
        if (tile.GetWater() <= 0)
        {
            switch (rand() % 10)
            {
            case 1:
                UnitList::NewUnit(tile, mZebra);
                break;
            case 6:
                UnitList::NewUnit(tile, mGrass);
                break;
            }
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
    LOG(INFO) << "Connecting to " << aAddress << ":" << aPort;

    boost::thread connectionManager(ConnectionManager, boost::ref(*this), aAddress, aPort);

    while (true)
    {
        LOG(INFO) << "Whait...";
        mTimer.Wait();
        UpdateGame();
    }
    LOG(INFO) << "Game over";
}

void ServerGame::UpdateGame()
{
    boost::lock_guard<boost::shared_mutex> cs(mGameMutex);

    LOG(INFO) << "Update Game!";

    MindList::UpdateMinds(mTimeStep);

    for (UnitListIterator i = UnitList::GetIterator(); !i.IsDone(); i.Next())
    {
        ServerUnit* unit = i.GetUnit();
        if (unit->UpdateAgeAndIsTimeToDie(mTimeStep))
        {
            UnitList::DeleteUnit(unit->GetUnitId());
        }
    }

    mTime += mTimeStep;
    for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
    {
        (*i)->GetChangeList()->Commit();
    }

    LOG(INFO) << "Time: " << mTime;
}


