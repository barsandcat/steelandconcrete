#include <pch.h>
#include <ServerGame.h>

#include <Network.h>
#include <ConnectionManager.h>
#include <ChangeList.h>
#include <VisualCodes.h>
#include <UnitList.h>
#include <UnitListIterator.h>
#include <MindList.h>

DEFINE_int32(update_length, 1000, "Time in milliseconds between game updates");
DEFINE_int32(time_step, 1, "Amount on which time advance on each update");

GameTime ServerGame::mTime = 1;

GameTime ServerGame::GetTime()
{
    return mTime;
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
    mTimer(FLAGS_update_length)
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

void ServerGame::Update()
{
    LOG(INFO) << "Whait...";
    mTimer.Wait();

    boost::lock_guard<boost::shared_mutex> cs(mGameMutex);

    LOG(INFO) << "Update Game!";

    MindList::UpdateMinds(FLAGS_time_step);

    mTime += FLAGS_time_step;
    for (ServerGeodesicGrid::Tiles::const_iterator i = mTiles.begin(); i != mTiles.end(); ++i)
    {
        (*i)->GetChangeList()->Commit();
    }

    LOG(INFO) << "Time: " << mTime;
}


