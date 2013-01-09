#ifndef CLIENTFOV_H
#define CLIENTFOV_H

#include<INetwork.h>
#include<ServerGame.h>
#include<Typedefs.h>
#include<IChange.h>
#include<boost/noncopyable.hpp>


void AddShowTile(PayloadMsg& aResponse, TileId aTileId, const ServerGeodesicGrid::Tiles& aTiles);

void AddHideTile(PayloadMsg& aResponse, TileId aTileId);

class ClientFOV: public boost::noncopyable
{
public:
    ClientFOV(INetwork& aNetwork, const ServerGeodesicGrid::Tiles& aTiles, UnitId aAvatarId);
    ~ClientFOV();
    void WritePartialUpdate(const int32 toSend, const int32 aVisionRadius);
    void WriteFullUpdate(const int32 aVisionRadius);
    void WriteFinalMessage(const GameTime aServerTime, const int32 aGameUpdateLength);
private:
    std::set<TileId> GetVisibleTiles(int aDepth);
    const UnitId mAvatarId;
    INetwork& mNetwork;
    const ServerGeodesicGrid::Tiles& mTiles;
    std::set<TileId> mVisibleTiles;

};

#endif // CLIENTFOV_H
