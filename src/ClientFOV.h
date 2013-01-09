#ifndef CLIENTFOV_H
#define CLIENTFOV_H

#include<INetwork.h>
#include<ServerGame.h>
#include<Typedefs.h>
#include<IChange.h>
#include<boost/noncopyable.hpp>

class ClientFOV: public boost::noncopyable
{
public:
    ClientFOV(INetwork& aNetwork, const ServerGeodesicGrid::Tiles& aTiles, UnitId aAvatarId);
    ~ClientFOV();
    void SendUpdate(const GameTime aServerTime, const GameTime aClientTime,
                           const GameTime aTimeStep, const int32 aVisionRadius,
                           const int32 aGameUpdateLength);
private:
    std::set<TileId> GetVisibleTiles(int aDepth);
    void AddShowTile(PayloadMsg& aResponse, TileId aTileId);
    const UnitId mAvatarId;
    INetwork& mNetwork;
    const ServerGeodesicGrid::Tiles& mTiles;
    std::set<TileId> mVisibleTiles;

};

#endif // CLIENTFOV_H
