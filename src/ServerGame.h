#ifndef SERVERAPP_H
#define SERVERAPP_H

#include <Typedefs.h>
#include <ServerGeodesicGrid.h>
#include <ServerUnit.h>
#include <Request.pb.h>
#include <boost/thread.hpp>

class ServerGame: public boost::noncopyable
{
public:
    typedef std::map< UnitId, ServerUnit* > ServerUnits;
    ServerGame(int aSize, int32 aSeaLevel);
    ~ServerGame();
    void MainLoop(Ogre::String aAddress, int32 aPort);
    ServerGeodesicGrid& GetGrid();
    ServerUnit& CreateUnit(ServerTile& aTile, const UnitClass& aClass);
    void Send(Network& aNetwork);
    GameTime GetTime() const { return mTime; }
    void LoadCommands(const RequestMsg& commands);
	int32 GetUpdateLength() const { return mUpdateLength; }
protected:
private:
    void UpdateGame();
    ServerGeodesicGrid* mGrid;
    int mUnitCount;
    GameTime mTime;
    GameTime mTimeStep;
    ServerUnits mUnits;
    UnitClass mGrass;
    UnitClass mZebra;
    UnitClass mAvatar;
    boost::mutex mGameMutex;
	int32 mUpdateLength;
};

#endif // SERVERAPP_H
