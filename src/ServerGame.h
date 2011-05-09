#ifndef SERVERAPP_H
#define SERVERAPP_H

#include <Typedefs.h>
#include <ServerGeodesicGrid.h>
#include <ServerUnit.h>
#include <Request.pb.h>
#include <boost/thread.hpp>
#include <UpdateTimer.h>

class ServerGame: public boost::noncopyable
{
public:
    ServerGame(int32 aSize);
    ~ServerGame();
    void MainLoop(Ogre::String aAddress, int32 aPort);
    static GameTime GetTime();
    static GameTime GetTimeStep();
    void LoadCommands(const RequestMsg& commands);
	int32 GetUpdateLength() { return mTimer.GetLeft(); }
	const ServerGeodesicGrid::Tiles& GetTiles() const { return mTiles; }
	int32 GetSize() const { return mSize; }
private:
    void UpdateGame();
    ServerGeodesicGrid::Tiles mTiles;
    int32 mSize;
    static GameTime mTime;
    static GameTime mTimeStep;
    UnitClass mGrass;
    UnitClass mZebra;
    UnitClass mAvatar;
    boost::shared_mutex mGameMutex;
	UpdateTimer mTimer;
};

#endif // SERVERAPP_H
