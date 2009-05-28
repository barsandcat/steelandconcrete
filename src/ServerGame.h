#ifndef SERVERAPP_H
#define SERVERAPP_H

#include <ServerGeodesicGrid.h>
#include <ClientConnection.h>
#include <ServerUnit.h>
#include <sockio.h>
#include <task.h>

class ServerGame
{
public:
    typedef std::map< int, ServerUnit* > ServerUnits;
    ServerGame();
    ~ServerGame();
    void MainLoop();
    ServerGeodesicGrid& GetGrid();
    ServerUnit& CreateUnit(ServerTile& mTile);
    void Send(socket_t& aSocket) const;
    int GetTime() const { return mTime; }
    void SignalClientEvent() { mClientEvent->signal(); }
protected:
private:
    void UpdateGame();
    ServerGeodesicGrid* mGrid;
    int mUnitCount;
    int mTime;
    ServerUnits mUnits;
    event* mClientEvent;
};

#endif // SERVERAPP_H
