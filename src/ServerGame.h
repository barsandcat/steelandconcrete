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
    ServerGame(int aSize, int32 aSeaLevel);
    ~ServerGame();
    void MainLoop(Ogre::String aAddress, int32 aPort);
    ServerGeodesicGrid& GetGrid();
    void Send(Network& aNetwork);
    UnitId AssignAvatar();
    void FreeAvatar(UnitId aAvatar);
    static GameTime GetTime();
    static GameTime GetTimeStep();
    void LoadCommands(const RequestMsg& commands);
	int32 GetUpdateLength() { return mTimer.GetLeft(); }
protected:
private:
    void UpdateGame();
    ServerGeodesicGrid* mGrid;
    static GameTime mTime;
    static GameTime mTimeStep;
    UnitClass mGrass;
    UnitClass mZebra;
    UnitClass mAvatar;
    boost::shared_mutex mGameMutex;
	UpdateTimer mTimer;
};

#endif // SERVERAPP_H
