#ifndef SERVERGAME_H
#define SERVERGAME_H

#include <Typedefs.h>
#include <ServerGeodesicGrid.h>
#include <ServerUnit.h>
#include <Payload.pb.h>
#include <boost/thread.hpp>
#include <UpdateTimer.h>

DECLARE_int32(time_step);

class ServerGame: public boost::noncopyable
{
public:
    ServerGame(int32 aSize);
    ~ServerGame();
    void MainLoop(Ogre::String aAddress, int32 aPort);
    static GameTime GetTime();
	int32 GetUpdateLength() { return mTimer.GetLeft(); }
	const ServerGeodesicGrid::Tiles& GetTiles() const { return mTiles; }
	int32 GetSize() const { return mSize; }
	boost::shared_mutex& GetGameMutex() { return mGameMutex; }
    void Update();
private:
    ServerGeodesicGrid::Tiles mTiles;
    int32 mSize;
    static GameTime mTime;
    UnitClass mGrass;
    UnitClass mZebra;
    UnitClass mAvatar;
    boost::shared_mutex mGameMutex;
	UpdateTimer mTimer;
};

#endif // SERVERGAME_H
